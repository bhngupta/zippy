#include "Database.h"

void Database::set(const std::string& key, const std::string& value) {
    hashTable.set(key, value);
}

std::string Database::get(const std::string& key) {
    return hashTable.get(key);
    // Return empty string if key not found
}

void Database::del(const std::string& key){
    hashTable.del(key);
}