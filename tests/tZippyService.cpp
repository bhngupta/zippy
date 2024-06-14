#include "ClientHandler.h"
#include "Database.h"
#include "ZippyClient.h"
#include "ZippyService.h"

#include <grpcpp/grpcpp.h>
#include <gtest/gtest.h>

class ZippyServiceTest : public ::testing::Test {
protected:
  void SetUp() override {
    db = new Database(10);
    service = new ZippyService(*db);

    grpc::ServerBuilder builder;
    std::string server_address("0.0.0.0:50052");
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(service);
    service->cq_ = builder.AddCompletionQueue();
    server = builder.BuildAndStart();
    for (int i = 0; i < 3; ++i) {
      service->threads_.emplace_back(&ZippyService::HandleRpcs, service);
    }
  }

  void TearDown() override {
    server->Shutdown();
    service->cq_->Shutdown();
    for (auto &thread : service->threads_) {
      thread.join();
    }
    delete service;
    delete db;
  }

  void SetCommand(ZippyClient &client, const std::string &key, const std::string &value) {
    std::string command = "SET " + key + " " + value;
    std::string reply = client.ExecuteCommand(command);
    EXPECT_EQ(reply, "Set operation performed successfully");
  }

  void GetCommand(ZippyClient &client, const std::string &key, const std::string &expected_value) {
    std::string command = "GET " + key;
    std::string reply = client.ExecuteCommand(command);
    EXPECT_EQ(reply, "Retrieved value: " + expected_value);
  }

  void DelCommand(ZippyClient &client, const std::string &key) {
    std::string command = "DEL " + key;
    std::string reply = client.ExecuteCommand(command);
    EXPECT_EQ(reply, "Delete operation performed successfully");
  }

  ZippyService *service;
  Database *db;
  std::unique_ptr<grpc::Server> server;
};

// Concurrency test case
TEST_F(ZippyServiceTest, ConcurrencyTest) {
  const int num_clients = 3;
  const int num_operations = 6;

  std::vector<std::thread> client_threads;
  std::vector<std::unique_ptr<ZippyClient>> clients;

  for (int i = 0; i < num_clients; ++i) {
    clients.push_back(std::make_unique<ZippyClient>(
        grpc::CreateChannel("localhost:50052", grpc::InsecureChannelCredentials())));
  }

  for (int i = 0; i < num_clients; ++i) {
    client_threads.emplace_back([&, i]() {
      for (int j = 0; j < num_operations; ++j) {
        std::string key = "key_" + std::to_string(i) + "_" + std::to_string(j);
        std::string value = "value_" + std::to_string(i) + "_" + std::to_string(j);
        SetCommand(*clients[i], key, value);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        GetCommand(*clients[i], key, value);
        DelCommand(*clients[i], key);
      }
    });
  }

  for (auto &thread : client_threads) {
    thread.join();
  }
}

// Single client test case
TEST_F(ZippyServiceTest, SingleClientTest) {
  ZippyClient client(grpc::CreateChannel("localhost:50052", grpc::InsecureChannelCredentials()));

  std::string key = "key_1";
  std::string value = "value_1";

  SetCommand(client, key, value);
  GetCommand(client, key, value);
  DelCommand(client, key);
}

// Edge case test for setting and deleting the same key repeatedly
TEST_F(ZippyServiceTest, RepeatedSetAndDeleteTest) {
  ZippyClient client(grpc::CreateChannel("localhost:50052", grpc::InsecureChannelCredentials()));

  std::string key = "key_edge";
  std::string value = "value_edge";

  for (int i = 0; i < 100; ++i) {
    SetCommand(client, key, value);
    GetCommand(client, key, value);
    DelCommand(client, key);
  }
}
