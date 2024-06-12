#ifndef DATABASE_H
#define DATABASE_H

#include "HashTable.h"
#include <chrono>
#include <unordered_map>

class Database {
public:
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    void del(const std::string& key);

private:
    HashTable hashTable;
    std::unordered_map<std::string, std::chrono::time_point<std::chrono::steady_clock>> expirationMap;
};

#endif // DATABASE_H

