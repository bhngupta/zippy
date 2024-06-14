#include "HashTable.h"

void HashTable::set(const std::string &key, const std::string &value) {
  table[key] = value;
}

std::string HashTable::get(const std::string &key) {
  if (table.find(key) != table.end()) {
    return table[key];
  }
  // This returns "" -> Have to handle this in the client side
  return "";
}

void HashTable::del(const std::string &key) {
  table.erase(key);
}

bool HashTable::exists(const std::string &key) {
  return table.find(key) != table.end();
}

size_t HashTable::size() const {
  return table.size();
}