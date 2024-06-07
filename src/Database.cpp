#include "Database.h"
#include <iostream>

Database::Database(size_t capacity, int ttl) : capacity(capacity), ttl(ttl), running_(false) {}

Database::~Database() {
    stopBackgroundThread();
}

void Database::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // New expiration set => Time Now + TTL
    auto now = std::chrono::steady_clock::now();
    auto expiration = now + std::chrono::seconds(ttl);

    // Insert or Update happens 
    hashTable.set(key, value);
    cache[key] = {value, expiration};

    std::cout << "SET > key: " << key << std::endl;
    

    // Update LRU cache
    if (lruMap.find(key) != lruMap.end()) {
        lruList.erase(lruMap[key]);
    }
    lruList.push_front(key);
    lruMap[key] = lruList.begin();

    maintainCapacity();
}

std::string Database::get(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    //TODO - Need to check in registry before
    //TODO - Change returns "" if not in hashtable 

    auto it = cache.find(key);
    if (it == cache.end()) {
        std::cout << "GET key: " << key << " not found" << std::endl;
        return "";
    }

    auto now = std::chrono::steady_clock::now();
    if (it->second.expiration <= now) {
        std::cout << "GET key: " << key << " expired" << std::endl;
        cache.erase(key);
        lruList.erase(lruMap[key]);
        lruMap.erase(key);
        return "";
    }

    auto value = it->second.value;
    it->second.expiration = now + std::chrono::seconds(ttl);  // Update expiration time

    // Update LRU list
    lruList.erase(lruMap[key]);
    lruList.push_front(key);
    lruMap[key] = lruList.begin();

    std::cout << "GET key: " << key << " value: " << value << std::endl;
    return value;
}

void Database::del(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (hashTable.exists(key)) {
        hashTable.del(key);
        cache.erase(key);
        
        if (lruMap.find(key) != lruMap.end()) {
            lruList.erase(lruMap[key]);
            lruMap.erase(key);
        }
        std::cout << "Deleted key: " << key << std::endl;
    } else {
        std::cout << "Key: " << key << " does not exist" << std::endl;
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
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        {
            std::lock_guard<std::mutex> lock(mutex_);
            purgeExpired();
        }
    }
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
            break; // LRU list is in descending order of access time, so no need to check further
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