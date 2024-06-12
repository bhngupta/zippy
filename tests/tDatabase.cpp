#include <gtest/gtest.h>
#include "Database.h"

// Test fixture for Database class
class DatabaseTest : public ::testing::Test {
protected:
    // Member variables
    Database db;

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
