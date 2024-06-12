#ifndef DATABASE_H
#define DATABASE_H

#include "HashTable.h"
#include <chrono>
#include <list>
#include <unordered_map>

class Database {
public:
    Database(size_t capacity, int ttl = 600); // Default TTL of 600 seconds (10 minutes)
    
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void del(const std::string& key);
    void removeExpiredEntries();
    
private:
    void evictIfNecessary();
    
    HashTable hashTable;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> expirationMap;
    std::list<std::string> lruList;
    std::unordered_map<std::string, std::list<std::string>::iterator> lruMap;
    size_t capacity;
    int ttl; // Time to live in seconds
};

#endif // DATABASE_H
