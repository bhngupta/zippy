#include "ZippyService.h"
#include "ClientHandler.h"
#include <sstream>
#include <string>
#include <iostream>
#include <uuid/uuid.h>

ZippyService::ZippyService(Database& db) : db_(db), shutdown_(false) {
    server_ = nullptr;
    cq_ = nullptr;
}

ZippyService::~ZippyService() {
    std::cout << "ZippyService instance destroyed" << std::endl;
    Shutdown();
}

void ZippyService::Shutdown() {
    if (!shutdown_) {
        shutdown_ = true;
        if (server_) {
            server_->Shutdown();
        }
        if (cq_) {
            cq_->Shutdown();
        }
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
}

grpc::Status ZippyService::ExecuteCommand(grpc::ServerContext* context, const zippy::CommandRequest* request, zippy::CommandResponse* response) {
    std::string command = request->command();
    std::string client_id = request->client_id();
    std::istringstream iss(command);
    std::string arg;
    iss >> arg;

    if (arg == "SET") {
        std::string key, value;
        iss >> key >> value;
        db_.set(key, value);
        response->set_result("Set operation performed successfully");
        log(client_id, arg, key, value);
    } else if (arg == "GET") {
        std::string key;
        iss >> key;
        std::string retrievedValue = db_.get(key);
        response->set_result("Retrieved value: " + retrievedValue);
        log(client_id, arg, key, retrievedValue);
    } else if (arg == "DEL") {
        std::string key;
        iss >> key;
        db_.del(key);
        response->set_result("Delete operation performed successfully");
        log(client_id, arg, key);
    } else {
        response->set_result("Invalid command");
    }
    return grpc::Status::OK;
}

grpc::Status ZippyService::GenerateUUID(grpc::ServerContext* context, const zippy::UUIDRequest* request, zippy::UUIDResponse* response) {
    uuid_t uuid;
    uuid_generate_random(uuid);
    char uuid_str[6]; // UUID string length is 36 characters + null terminator
    uuid_unparse(uuid, uuid_str);
    response->set_client_id(uuid_str);
    std::cout << "Generated UUID: " << uuid_str << std::endl;
    return grpc::Status::OK;
}

void ZippyService::log(const std::string& client_id, const std::string& operation, const std::string& key, const std::string& value) {
    if (operation == "SET") {
        std::cout << "Client: " << client_id << ", Operation: " << operation << ", Key: " << key << ", Value: " << value << std::endl;
    } else if (operation == "GET") {
        if (value.empty()) {
            std::cout << "Client: " << client_id << ", Operation: " << operation << ", Key: " << key << ", Result: Key not found" << std::endl;
        } else {
            std::cout << "Client: " << client_id << ", Operation: " << operation << ", Key: " << key << ", Value: " << value << std::endl;
        }
    } else if (operation == "DEL") {
        std::cout << "Client: " << client_id << ", Operation: " << operation << ", Key: " << key << std::endl;
    } else {
        std::cout << "Client: " << client_id << ", Operation: " << operation << ", Command: " << key << std::endl;
    }
}

void ZippyService::HandleRpcs() {
    new ClientHandler(&db_, this, cq_.get());
    void* tag;
    bool ok;

    while (true) {
        std::cout << "Waiting for next event in completion queue" << std::endl;
        if (cq_->Next(&tag, &ok)) {
            if (ok) {
                std::cout << "Processing tag" << std::endl;
                static_cast<ClientHandler*>(tag)->Proceed();
            } else {
                std::cout << "Deleting tag" << std::endl;
                delete static_cast<ClientHandler*>(tag);
            }
        } else {
            std::cout << "Failed to get next event, shutting down HandleRpcs loop" << std::endl;
            break;
        }
    }
    std::cout << "Exited HandleRpcs loop" << std::endl;
}