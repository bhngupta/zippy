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
    db.set("key2", "value2");

    EXPECT_EQ(db.get("key1"), "value1");
    EXPECT_EQ(db.get("key2"), "value2");
}

// Additional test cases can be added as needed

// Main function to run all tests
int test_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
