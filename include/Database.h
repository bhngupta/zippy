#ifndef DATABASE_H
#define DATABASE_H

#include "HashTable.h"
#include "SnapshotManager.h"

#include <atomic>
#include <chrono>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

class Database {
public:
  Database(size_t capacity, int ttl = 600,
           std::chrono::milliseconds snapshotInterval = std::chrono::minutes(5),
           const std::string& snapshotFilePath = "../snapshot.bin");
  ~Database();

  void set(const std::string &key, const std::string &value);
  std::string get(const std::string &key);
  void del(const std::string &key);

  void startBackgroundThread();
  void stopBackgroundThread();

  void forceSnapshot();

private:
  void backgroundTask();
  void purgeExpired();
  void maintainCapacity();
  void updateExpiration(const std::string &key);
  void takeSnapshot();

  struct CacheEntry {
    std::string value;
    std::chrono::steady_clock::time_point expiration;
  };

  size_t capacity;
  int ttl;

  HashTable hashTable;
  SnapshotManager snapshotManager;

  std::mutex mutex_;
  std::atomic<bool> running_;
  std::thread backgroundThread_;
  std::list<std::string> lruList;
  std::chrono::milliseconds snapshotInterval;
  std::unordered_map<std::string, CacheEntry> cache;
  std::unordered_map<std::string, std::list<std::string>::iterator> lruMap;
};

#endif
