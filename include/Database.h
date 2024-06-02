// Classes and methods for Database Header
#ifndef DATABASE_H
#define DATABASE_H

#include "HashTable.h"

class Database {
public:
    void set(const std::string& key, const std::string& value);
    
    std::string get(const std::string& key);

private:
    HashTable hashTable;
};

#endif // DATABASE_H
