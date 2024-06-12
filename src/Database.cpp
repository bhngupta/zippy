#include "Database.h"
#include <iostream>

const int FIXED_TTL = 25; // Fixed TTL of 25 seconds

void Database::set(const std::string& key, const std::string& value) {
    hashTable.set(key, value);
    expirationMap[key] = std::chrono::steady_clock::now() + std::chrono::seconds(FIXED_TTL);
    std::cout << "Set key: " << key << " with TTL: " << FIXED_TTL << " seconds" << std::endl;
}

std::string Database::get(const std::string& key) {
    auto it = expirationMap.find(key);
    if (it != expirationMap.end()) {
        if (std::chrono::steady_clock::now() > it->second) {
            std::cout << "Key: " << key << " has expired" << std::endl;
            del(key);
            return "";
        }
    }
    std::string value = hashTable.get(key);
    std::cout << "Get key: " << key << ", value: " << value << std::endl;
    return value;
}

void Database::del(const std::string& key) {
    std::cout << "Attempting to delete key: " << key << std::endl;
    hashTable.del(key);
    expirationMap.erase(key);
    std::cout << "Deleted key: " << key << std::endl;
}
