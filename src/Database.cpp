#include "Database.h"

#include <iostream>

Database::Database(size_t capacity, int ttl, std::chrono::milliseconds snapshotInterval, const std::string& snapshotFilePath)
    : capacity(capacity), ttl(ttl), snapshotManager(snapshotFilePath), running_(false), snapshotInterval(snapshotInterval) {
  auto snapshot = snapshotManager.loadSnapshot();
  for (const auto &pair : snapshot) {
    cache[pair.first] = {pair.second, std::chrono::steady_clock::time_point::max()};
  }
}

Database::~Database() {
  // Default constructor on end stops the backgroud threads - indierctly used by purgeExpired()
  // function
  stopBackgroundThread();
}

void Database::set(const std::string &key, const std::string &value) {
  std::lock_guard<std::mutex> lock(mutex_);

  // New expiration set => Time Now + TTL
  auto now = std::chrono::steady_clock::now();
  auto expiration = now + std::chrono::seconds(ttl);

  // Insert or Update
  // FEAT: UPDATE can be sent into a different method?
  hashTable.set(key, value);
  cache[key] = {value, expiration};

  std::cout << "SET > key: " << key << std::endl;

  // Updating LRU cache
  if (lruMap.find(key) != lruMap.end()) {
    lruList.erase(lruMap[key]);
  }
  lruList.push_front(key);
  lruMap[key] = lruList.begin();

  maintainCapacity();
}

std::string Database::get(const std::string &key) {
  std::lock_guard<std::mutex> lock(mutex_);

  auto it = cache.find(key);
  if (it != cache.end()) {
    auto now = std::chrono::steady_clock::now();
    if (it->second.expiration > now) {
      it->second.expiration = now + std::chrono::seconds(ttl);

      lruList.erase(lruMap[key]);
      lruList.push_front(key);
      lruMap[key] = lruList.begin();

      std::cout << "GET key: " << key << " value: " << it->second.value << std::endl;
      return it->second.value;
    } else {
      cache.erase(key);
      lruList.erase(lruMap[key]);
      lruMap.erase(key);
    }
  }

  auto value = snapshotManager.getValueFromSnapshot(key);
  if (!value.empty()) {
    auto expiration = std::chrono::steady_clock::now() + std::chrono::seconds(ttl);
    cache[key] = {value, expiration};
    hashTable.set(key, value);

    lruList.push_front(key);
    lruMap[key] = lruList.begin();

    std::cout << "GET key: " << key << " value from snapshot: " << value << std::endl;
    return value;
  }

  std::cout << "GET key: " << key << " not found" << std::endl;
  return "";
}

void Database::del(const std::string &key) {
  std::lock_guard<std::mutex> lock(mutex_);

  bool inMemory = hashTable.exists(key);

  if (inMemory) {
    hashTable.del(key);
    cache.erase(key);
    if (lruMap.find(key) != lruMap.end()) {
      lruList.erase(lruMap[key]);
      lruMap.erase(key);
    }
    std::cout << "Deleted key: " << key << std::endl;
  }

  // Attempt to delete from snapshot
  auto value = snapshotManager.getValueFromSnapshot(key);
  if(!value.empty()){
    snapshotManager.deleteKeyFromSnapshot(key);
  }
  else if (!inMemory){
    std::cout << "Key: "<< "does not exist"<<std::endl;  
  }
}

void Database::startBackgroundThread() {
  running_ = true;
  backgroundThread_ = std::thread(&Database::backgroundTask, this);
}

void Database::stopBackgroundThread() {
  running_ = false;
  if (backgroundThread_.joinable()) {
    backgroundThread_.join();
  }
}

// || Helper Functions ||

void Database::backgroundTask() {
  auto lastSnapshotTime = std::chrono::steady_clock::now();
  while (running_) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    {
      std::lock_guard<std::mutex> lock(mutex_);
      purgeExpired();
    }

    auto now = std::chrono::steady_clock::now();
    if (now - lastSnapshotTime >= std::chrono::minutes(2)) {
      std::cout << "Taking snapshot at "
                << std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count()
                << " seconds since epoch" << std::endl;
      takeSnapshot();
      lastSnapshotTime = now;
    }
  }
}

void Database::takeSnapshot() {
  std::lock_guard<std::mutex> lock(mutex_);

  // Convert cache to a format suitable for SnapshotManager
  std::unordered_map<std::string, std::string> currentSnapshot;
  for (const auto &pair : cache) {
    currentSnapshot[pair.first] = pair.second.value;
  }

  auto snapshot = snapshotManager.loadSnapshot();
  auto mergedSnapshot = snapshotManager.mergeSnapshots(snapshot, currentSnapshot);
  snapshotManager.saveSnapshot(mergedSnapshot);

  auto now = std::chrono::steady_clock::now();
  std::cout << "Snapshot taken at "
            << std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count()
            << " seconds since epoch" << std::endl;
}

void Database::forceSnapshot() {
  takeSnapshot();
}

void Database::purgeExpired() {
  auto now = std::chrono::steady_clock::now();
  for (auto it = lruList.rbegin(); it != lruList.rend(); ++it) {
    auto key = *it;
    auto entry = cache[key];
    if (entry.expiration <= now) {
      cache.erase(key);
      lruMap.erase(key);
      lruList.erase(std::next(it).base());
      std::cout << "Removed expired key from cache: " << key << std::endl;

      if (hashTable.exists(key)) {
        hashTable.del(key);
        std::cout << "Deleted key: " << key << std::endl;
      }
    } else {
      break; // LRU list is in descending order of access time, so no need to
             // check further
    }
  }
}

void Database::maintainCapacity() {
  while (hashTable.size() > capacity) {
    std::string key = lruList.back();
    lruList.pop_back();
    hashTable.del(key);
    cache.erase(key);
    lruMap.erase(key);
    std::cout << "Evicted key: " << key << " due to capacity limits" << std::endl;
  }
}