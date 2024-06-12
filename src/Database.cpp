#include "Database.h"
#include <iostream>

Database::Database(size_t capacity, int ttl) : capacity(capacity), ttl(ttl) {}

void Database::set(const std::string& key, const std::string& value) {
    hashTable.set(key, value);
    expirationMap[key] = std::chrono::steady_clock::now() + std::chrono::seconds(ttl);
    std::cout << "Set key: " << key << " with TTL: " << ttl << " seconds" << std::endl;

    // Update LRU cache
    if (lruMap.find(key) != lruMap.end()) {
        lruList.erase(lruMap[key]);
    }
    lruList.push_front(key);
    lruMap[key] = lruList.begin();
    evictIfNecessary();
}

std::string Database::get(const std::string& key) {
    auto it = expirationMap.find(key);
    if (it != expirationMap.end() && std::chrono::steady_clock::now() > it->second) {
        std::cout << "Key: " << key << " has expired" << std::endl;
        hashTable.del(key);
        expirationMap.erase(it);
        if (lruMap.find(key) != lruMap.end()) {
            lruList.erase(lruMap[key]);
            lruMap.erase(key);
        }
        return "";
    }

    std::string value = hashTable.get(key);
    if (hashTable.exists(key)) {
        lruList.erase(lruMap[key]);
        lruList.push_front(key);
        lruMap[key] = lruList.begin();
    }

    std::cout << "Get key: " << key << ", value: " << value << std::endl;
    return value;
}

void Database::del(const std::string& key) {
    std::cout << "Attempting to delete key: " << key << std::endl;
    if (hashTable.exists(key)) {
        hashTable.del(key);
        expirationMap.erase(key);
        if (lruMap.find(key) != lruMap.end()) {
            lruList.erase(lruMap[key]);
            lruMap.erase(key);
        }
        std::cout << "Deleted key: " << key << std::endl;
    } else {
        std::cout << "Key: " << key << " does not exist" << std::endl;
    }
}

void Database::removeExpiredEntries() {
    auto now = std::chrono::steady_clock::now();
    for (auto it = expirationMap.begin(); it != expirationMap.end();) {
        if (now > it->second) {
            std::string key = it->first;
            std::cout << "Key: " << key << " has expired" << std::endl;
            hashTable.del(key);
            if (lruMap.find(key) != lruMap.end()) {
                lruList.erase(lruMap[key]);
                lruMap.erase(key);
            }
            it = expirationMap.erase(it);
        } else {
            ++it;
        }
    }
}

void Database::evictIfNecessary() {
    while (lruList.size() > capacity) {
        std::string key = lruList.back();
        lruList.pop_back();
        lruMap.erase(key);
        hashTable.del(key);
        expirationMap.erase(key);
        std::cout << "Evicted key: " << key << " due to capacity limits" << std::endl;
    }
}
