#include <gtest/gtest.h>
#include "Database.h"
#include <thread>  // For sleep_for

// Test fixture for Database class
class DatabaseTest : public ::testing::Test {
protected:
    // Member variables
    Database db;

    // Constructor to initialize Database with capacity and a short TTL for testing
    DatabaseTest() : db(3, 5) {}  // Set a capacity of 3 for LRU cache and a TTL of 5 seconds for testing

    // Helper functions
    void SetUp() override {
        // Code to run before each test
    }

    void TearDown() override {
        // Code to run after each test
    }
};

// Test case for set and get methods
TEST_F(DatabaseTest, SetAndGet) {
    db.set("key1", "value1");
    EXPECT_EQ(db.get("key1"), "value1");

    db.set("key2", "value2");
    EXPECT_EQ(db.get("key2"), "value2");
}

// Test case for non-existing key
TEST_F(DatabaseTest, NonExistingKey) {
    EXPECT_EQ(db.get("non_existing"), "");
}

// Test case for deleting a key
TEST_F(DatabaseTest, DeleteKey) {
    db.set("key1", "value1");
    db.del("key1");
    EXPECT_EQ(db.get("key1"), "");
}

// Test case for expired key
TEST_F(DatabaseTest, ExpiredKey) {
    db.set("key1", "value1");
    std::this_thread::sleep_for(std::chrono::seconds(6));  // Sleep for longer than the TTL
    EXPECT_EQ(db.get("key1"), "");
}

// Test case for eviction policy (LRU)
TEST_F(DatabaseTest, EvictionPolicy) {
    db.set("key1", "value1");
    db.set("key2", "value2");
    db.set("key3", "value3");
    db.set("key4", "value4");  // This should evict key1 (the least recently used)

    EXPECT_EQ(db.get("key1"), "");  // key1 should have been evicted
    EXPECT_EQ(db.get("key2"), "value2");
    EXPECT_EQ(db.get("key3"), "value3");
    EXPECT_EQ(db.get("key4"), "value4");
}
