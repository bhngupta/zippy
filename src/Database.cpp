#include "Database.h"

void Database::set(const std::string& key, const std::string& value) {
    data[key] = value;
}

std::string Database::get(const std::string& key) const {
    auto it = data.find(key);
    if (it != data.end()) {
        return it->second;
    }
    return ""; // Return empty string if key not found
}