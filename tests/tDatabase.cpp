#include <gtest/gtest.h>
#include "Database.h"
#include <thread>
#include <chrono>

// Test fixture for Database class
class DatabaseTest : public ::testing::Test {
protected:

    void SetUp() override {
        db = std::make_unique<Database>(3, 2);
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
    std::this_thread::sleep_for(std::chrono::seconds(3)); // Wait for TTL to expire
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

TEST_F(DatabaseTest, UpdateExpirationTimeTest) {
    db->set("key4", "value4");
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for half TTL
    db->get("key4"); // Access should update expiration time
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait more than half TTL
    EXPECT_EQ(db->get("key4"), "value4"); // Should still be available as TTL was reset
}
