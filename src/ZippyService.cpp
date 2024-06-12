#include <sstream>
#include <string>
#include <iostream>
#include "ZippyService.h"

grpc::Status ZippyService::ExecuteCommand(grpc::ServerContext* context, const zippy::CommandRequest* request, zippy::CommandResponse* response) {
    std::string command = request->command();
    std::istringstream iss(command);
    std::string arg;
    iss >> arg;
    // std::string client = context->peer();

    if (arg == "SET") {
        std::string key, value;
        iss >> key >> value;
        db_.set(key, value);
        response->set_result("Set operation performed successfully");
        log(arg, key, value);
    } else if (arg == "GET") {
        std::string key;
        iss >> key;
        std::string retrievedValue = db_.get(key);
        response->set_result("Retrieved value: " + retrievedValue);
        log(arg, key, retrievedValue);
    } else if (arg == "DEL") {
        std::string key;
        iss >> key;
        db_.del(key);
        response->set_result("Delete operation performed successfully");
        log(arg, key);
    } else {
        response->set_result("Invalid command");
    }
    return grpc::Status::OK;
}

void ZippyService::log(const std::string& operation, const std::string& key, const std::string& value) {
    if (operation == "SET") {
        std::cout << "Client: " << "client-1" << ", Operation: " << operation << ", Key: " << key << ", Value: " << value << std::endl;
    } else if (operation == "GET") {
        if (value.empty()) {
            std::cout << "Client: " << "client-1" << ", Operation: " << operation << ", Key: " << key << ", Result: Key not found" << std::endl;
        } else {
            std::cout << "Client: " << "client-1" << ", Operation: " << operation << ", Key: " << key << ", Value: " << value << std::endl;
        }
    } else if (operation == "DEL") {
        std::cout << "Client: " << "client-1" << ", Operation: " << operation << ", Key: " << key << std::endl;
    } else {
        std::cout << "Client: " << "client-1" << ", Operation: " << operation << ", Command: " << key << std::endl;
    }
}
