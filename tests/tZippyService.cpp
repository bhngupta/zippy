// zippy_service_test.cpp
#include <gtest/gtest.h>
#include <grpcpp/grpcpp.h>
#include "Database.h"
#include "ZippyService.h"
#include "zippy.grpc.pb.h"
#include <thread>  // For sleep_for

class ZippyServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup the in-memory database with a shorter TTL for testing
        db = std::make_unique<Database>(3, 5); // Set a capacity of 3 for LRU cache and a TTL of 5 seconds for testing
        service = std::make_unique<ZippyService>(*db);
    }

    void TearDown() override {
        // Cleanup if needed
    }

    std::unique_ptr<Database> db;
    std::unique_ptr<ZippyService> service;
};

TEST_F(ZippyServiceTest, SetCommand) {
    grpc::ServerContext context;
    zippy::CommandRequest request;
    zippy::CommandResponse response;

    request.set_command("SET key1 value1");
    grpc::Status status = service->ExecuteCommand(&context, &request, &response);
    
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(response.result(), "Set operation performed successfully");
    EXPECT_EQ(db->get("key1"), "value1");
}

TEST_F(ZippyServiceTest, GetCommand) {
    grpc::ServerContext context;
    zippy::CommandRequest request;
    zippy::CommandResponse response;

    db->set("key1", "value1");

    request.set_command("GET key1");
    grpc::Status status = service->ExecuteCommand(&context, &request, &response);
    
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(response.result(), "Retrieved value: value1");
}

TEST_F(ZippyServiceTest, DelCommand) {
    grpc::ServerContext context;
    zippy::CommandRequest request;
    zippy::CommandResponse response;

    db->set("key1", "value1");

    request.set_command("DEL key1");
    grpc::Status status = service->ExecuteCommand(&context, &request, &response);
    
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(response.result(), "Delete operation performed successfully");
}

TEST_F(ZippyServiceTest, InvalidCommand) {
    grpc::ServerContext context;
    zippy::CommandRequest request;
    zippy::CommandResponse response;

    request.set_command("INVALID command");
    grpc::Status status = service->ExecuteCommand(&context, &request, &response);
    
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(response.result(), "Invalid command");
}

// // Test case for expired key
// TEST_F(ZippyServiceTest, ExpiredKey) {
//     grpc::ServerContext context;
//     zippy::CommandRequest request;
//     zippy::CommandResponse response;

//     request.set_command("SET key1 value1");
//     grpc::Status status = service->ExecuteCommand(&context, &request, &response);
    
//     EXPECT_TRUE(status.ok());
//     EXPECT_EQ(response.result(), "Set operation performed successfully");

//     std::this_thread::sleep_for(std::chrono::seconds(6));  // Sleep for longer than the TTL

//     request.set_command("GET key1");
//     status = service->ExecuteCommand(&context, &request, &response);

//     EXPECT_TRUE(status.ok());
//     EXPECT_EQ(response.result(), "Retrieved value: ");
// }

// // Test case for eviction policy (LRU)
// TEST_F(ZippyServiceTest, EvictionPolicy) {
//     grpc::ServerContext context;
//     zippy::CommandRequest request;
//     zippy::CommandResponse response;

//     request.set_command("SET key1 value1");
//     service->ExecuteCommand(&context, &request, &response);

//     request.set_command("SET key2 value2");
//     service->ExecuteCommand(&context, &request, &response);

//     request.set_command("SET key3 value3");
//     service->ExecuteCommand(&context, &request, &response);

//     request.set_command("SET key4 value4");  // This should evict key1 (the least recently used)
//     service->ExecuteCommand(&context, &request, &response);

//     request.set_command("GET key1");
//     grpc::Status status = service->ExecuteCommand(&context, &request, &response);
//     EXPECT_TRUE(status.ok());
//     EXPECT_EQ(response.result(), "Retrieved value: ");  // key1 should have been evicted

//     request.set_command("GET key2");
//     status = service->ExecuteCommand(&context, &request, &response);
//     EXPECT_TRUE(status.ok());
//     EXPECT_EQ(response.result(), "Retrieved value: value2");

//     request.set_command("GET key3");
//     status = service->ExecuteCommand(&context, &request, &response);
//     EXPECT_TRUE(status.ok());
//     EXPECT_EQ(response.result(), "Retrieved value: value3");

//     request.set_command("GET key4");
//     status = service->ExecuteCommand(&context, &request, &response);
//     EXPECT_TRUE(status.ok());
//     EXPECT_EQ(response.result(), "Retrieved value: value4");
// }
