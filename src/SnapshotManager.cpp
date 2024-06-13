#include "SnapshotManager.h"
#include <fstream>
#include <iostream>
#include <filesystem>

SnapshotManager::SnapshotManager(const std::string& snapshotFilePath)
    : snapshotFilePath_(snapshotFilePath) {}

void SnapshotManager::saveSnapshot(const std::unordered_map<std::string, std::string>& hashmap) {
    std::ofstream ofs(snapshotFilePath_, std::ios::binary | std::ios::trunc);
    if (!ofs) {
        std::cerr << "Unable to open snapshot file for writing: " << snapshotFilePath_ << std::endl;
        throw std::runtime_error("Unable to open snapshot file for writing");
    }

    size_t size = hashmap.size();
    std::cerr << "Saving snapshot with size: " << size << std::endl;
    ofs.write(reinterpret_cast<const char*>(&size), sizeof(size));

    for (const auto& pair : hashmap) {
        size_t keySize = pair.first.size();
        size_t valueSize = pair.second.size();
        ofs.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
        ofs.write(pair.first.data(), keySize);
        ofs.write(reinterpret_cast<const char*>(&valueSize), sizeof(valueSize));
        ofs.write(pair.second.data(), valueSize);
    }
    std::cerr << "Snapshot saved successfully." << std::endl;
}

std::unordered_map<std::string, std::string> SnapshotManager::loadSnapshot() {
    std::unordered_map<std::string, std::string> hashmap;

    // Check if file exists
    if (!std::filesystem::exists(snapshotFilePath_)) {
        std::cerr << "Snapshot file does not exist: " << snapshotFilePath_ << ". Creating a new snapshot file." << std::endl;
        saveSnapshot(hashmap); // Create an empty snapshot file
        return hashmap;
    }

    std::ifstream ifs(snapshotFilePath_, std::ios::binary);
    if (!ifs) {
        std::cerr << "Unable to open snapshot file for reading: " << snapshotFilePath_ << std::endl;
        return hashmap; // Return empty if snapshot does not exist
    }

    size_t size;
    ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::cerr << "Loaded snapshot with size: " << size << std::endl;

    for (size_t i = 0; i < size; ++i) {
        size_t keySize, valueSize;
        ifs.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        std::string key(keySize, ' ');
        ifs.read(&key[0], keySize);

        ifs.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
        std::string value(valueSize, ' ');
        ifs.read(&value[0], valueSize);

        hashmap[key] = value;
    }

    std::cerr << "Snapshot loaded successfully." << std::endl;
    return hashmap;
}

std::unordered_map<std::string, std::string> SnapshotManager::mergeSnapshots(
    const std::unordered_map<std::string, std::string>& currentSnapshot,
    const std::unordered_map<std::string, std::string>& newSnapshot) {

    std::unordered_map<std::string, std::string> mergedSnapshot = currentSnapshot;

    for (const auto& pair : newSnapshot) {
        mergedSnapshot[pair.first] = pair.second;
    }

    return mergedSnapshot;
}

std::string SnapshotManager::getValueFromSnapshot(const std::string& key) {
    auto snapshot = loadSnapshot();
    auto it = snapshot.find(key);
    if (it != snapshot.end()) {
        return it->second;
    }
    return "";
}

void SnapshotManager::deleteKeyFromSnapshot(const std::string& key) {
    auto snapshot = loadSnapshot();
    snapshot.erase(key);
    saveSnapshot(snapshot);
}
