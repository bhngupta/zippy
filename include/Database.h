// Classes and methods for Database Header
#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <unordered_map>

class Database {
public:
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key) const;

private:
    std::unordered_map<std::string, std::string> data;
};

#endif // DATABASE_H
