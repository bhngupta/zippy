#include <gtest/gtest.h>
#include "Database.h"
#include <thread>
#include <chrono>
#include <unordered_map>
#include <fstream>
// Test fixture for Database class
class DatabaseTest : public ::testing::Test {
protected:

    void SetUp() override {
        db = std::make_unique<Database>(3, 10, std::chrono::milliseconds(100));
        db->startBackgroundThread();
        // Code to run before each test
    }

    void TearDown() override {
        db->stopBackgroundThread();
        db.reset();
        // Code to run after each test
    }

    std::unique_ptr<Database> db;

};

// Test case for set and get methods
TEST_F(DatabaseTest, SetGetTest) {
    db->set("key1", "value1");
    EXPECT_EQ(db->get("key1"), "value1");
}

// Test case for deleting a key
TEST_F(DatabaseTest, DeletionTest) {
    db->set("key3", "value3");
    db->del("key3");
    EXPECT_EQ(db->get("key3"), ""); // Should return empty as the key has been deleted
}

// Test expiration
TEST_F(DatabaseTest, ExpirationTest) {
    db->set("key2", "value2");
    std::this_thread::sleep_for(std::chrono::seconds(11)); // Wait for TTL to expire
    EXPECT_EQ(db->get("key2"), ""); // Should return empty as the key has expired
}

// Test for LRU eviction
TEST_F(DatabaseTest, LRUEvictionTest) {
    db->set("key1", "value1");
    db->set("key2", "value2");
    db->set("key3", "value3");
    db->set("key4", "value4");

    // As key1 is the oldest, it should be evicted
    EXPECT_EQ(db->get("key1"), ""); 
    EXPECT_EQ(db->get("key4"), "value4");
}

// Test case for eviction policy (LRU)
TEST_F(DatabaseTest, LRUOrderTest) {
    db->set("key1", "value1");
    db->set("key2", "value2");
    db->set("key3", "value3");
    db->get("key1");
    db->set("key4", "value4");

    EXPECT_EQ(db->get("key2"), ""); // key2 should be evicted
    EXPECT_EQ(db->get("key1"), "value1");
    EXPECT_EQ(db->get("key4"), "value4");
}

// Additional test for snapshot functionality
TEST_F(DatabaseTest, SnapshotTest) {
    db->set("key1", "value1");
    db->set("key2", "value2");

    // Force snapshot save
    db->forceSnapshot();

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Short wait to ensure snapshot is saved

    std::ifstream ifs("./snapshot.bin", std::ios::binary);
    ASSERT_TRUE(ifs.is_open());

    size_t size;
    ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::cerr << "SnapshotTest: Loaded snapshot size: " << size << std::endl;
    EXPECT_EQ(size, 2);

    for (size_t i = 0; i < size; ++i) {
        size_t keySize, valueSize;
        ifs.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        std::string key(keySize, ' ');
        ifs.read(&key[0], keySize);

        ifs.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
        std::string value(valueSize, ' ');
        ifs.read(&value[0], valueSize);

        if (key == "key1") {
            EXPECT_EQ(value, "value1");
        } else if (key == "key2") {
            EXPECT_EQ(value, "value2");
        } else {
            FAIL() << "Unexpected key in snapshot";
        }
    }
}

TEST_F(DatabaseTest, SnapshotMergeTest) {
    db->set("key1", "value1");
    db->set("key2", "value2");

    // Force snapshot save
    db->forceSnapshot();

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Short wait to ensure snapshot is saved

    db->set("key2", "new_value2");
    db->set("key3", "value3");

    // Force another snapshot save
    db->forceSnapshot();

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Short wait to ensure snapshot is saved

    std::ifstream ifs("./snapshot.bin", std::ios::binary);
    ASSERT_TRUE(ifs.is_open());

    size_t size;
    ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::cerr << "SnapshotMergeTest: Loaded snapshot size: " << size << std::endl;
    EXPECT_EQ(size, 3);

    for (size_t i = 0; i < size; ++i) {
        size_t keySize, valueSize;
        ifs.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        std::string key(keySize, ' ');
        ifs.read(&key[0], keySize);

        ifs.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));
        std::string value(valueSize, ' ');
        ifs.read(&value[0], valueSize);

        if (key == "key1") {
            EXPECT_EQ(value, "value1");
        } else if (key == "key2") {
            EXPECT_EQ(value, "new_value2");
        } else if (key == "key3") {
            EXPECT_EQ(value, "value3");
        } else {
            FAIL() << "Unexpected key in snapshot";
        }
    }
}

// Test case for get method with snapshot fallback
TEST_F(DatabaseTest, GetFromSnapshotTest) {
    db->set("key1", "value1");
    db->set("key2", "value2");

    // Force snapshot save and clear the in-memory database
    db->forceSnapshot();
    db->del("key1");
    db->del("key2");

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Short wait to ensure snapshot is saved

    // Test get from snapshot
    EXPECT_EQ(db->get("key1"), "value1");
    EXPECT_EQ(db->get("key2"), "value2");
}

// Test case for delete method with snapshot fallback
TEST_F(DatabaseTest, DeleteFromSnapshotTest) {
    db->set("key1", "value1");
    db->set("key2", "value2");

    // Force snapshot save and clear the in-memory database
    db->forceSnapshot();
    db->del("key1");
    db->del("key2");

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Short wait to ensure snapshot is saved

    // Test delete from snapshot
    db->del("key1");
    EXPECT_EQ(db->get("key1"), ""); // Should return empty as the key has been deleted from the snapshot

    db->del("key2");
    EXPECT_EQ(db->get("key2"), ""); // Should return empty as the key has been deleted from the snapshot
}

TEST_F(DatabaseTest, UpdateExpirationTimeTest) {
    db->set("key4", "value4");
    std::this_thread::sleep_for(std::chrono::seconds(5)); // Wait for half TTL
    db->get("key4"); // Access should update expiration time
    std::this_thread::sleep_for(std::chrono::seconds(6)); // Wait more than half TTL
    EXPECT_EQ(db->get("key4"), "value4"); // Should still be available as TTL was reset
}
