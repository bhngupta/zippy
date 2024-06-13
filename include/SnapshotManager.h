#ifndef SNAPSHOTMANAGER_H
#define SNAPSHOTMANAGER_H

#include <unordered_map>
#include <string>

class SnapshotManager {
public:
    SnapshotManager(const std::string& snapshotFilePath);

    void saveSnapshot(const std::unordered_map<std::string, std::string>& hashmap);
    std::unordered_map<std::string, std::string> loadSnapshot();
    std::unordered_map<std::string, std::string> mergeSnapshots(
        const std::unordered_map<std::string, std::string>& currentSnapshot, 
        const std::unordered_map<std::string, std::string>& newSnapshot);
    std::string getValueFromSnapshot(const std::string& key);
    void deleteKeyFromSnapshot(const std::string& key);
private:
    std::string snapshotFilePath_;
};

#endif // SNAPSHOTMANAGER_H
