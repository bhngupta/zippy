// Classes and methods for Hashtable Header
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <unordered_map>
#include <string>

class HashTable {
public:

    void set(const std::string& key, const std::string& value);
    
    std::string get(const std::string& key);
    
    void del(const std::string& key);

private:
    std::unordered_map<std::string, std::string> table;
};

#endif
