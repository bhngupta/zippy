// zippy_service_test.cpp
#include <gtest/gtest.h>
#include <grpcpp/grpcpp.h>
#include "Database.h"
#include "ZippyService.h"
#include "zippy.grpc.pb.h"

class ZippyServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup the in-memory database
        db = std::make_unique<Database>();
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
