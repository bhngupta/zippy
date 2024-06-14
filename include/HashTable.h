#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <unordered_map>

class HashTable {
public:
  void set(const std::string &key, const std::string &value);
  std::string get(const std::string &key);
  void del(const std::string &key);
  bool exists(const std::string &key);
  size_t size() const;

private:
  std::unordered_map<std::string, std::string> table;
};

#endif
