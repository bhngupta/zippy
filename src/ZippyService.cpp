#include <sstream>
#include <string>
#include <iostream>
#include "ZippyService.h"

grpc::Status ZippyService::ExecuteCommand(grpc::ServerContext* context, const zippy::CommandRequest* request, zippy::CommandResponse* response) {
    std::string command = request->command();
    std::istringstream iss(command);
    std::string arg;
    iss >> arg;

    if (arg == "SET") {
        std::string key, value;
        iss >> key >> value;
        db_.set(key, value);
        response->set_result("Set operation performed successfully");
    } else if (arg == "GET") {
        std::string key;
        iss >> key;
        std::string retrievedValue = db_.get(key);
        response->set_result("Retrieved value: " + retrievedValue);
    } else if (arg == "DEL") {
        std::string key;
        iss >> key;
        db_.del(key);
        response->set_result("Delete operation performed successfully");
    } else {
        response->set_result("Invalid command");
    }
    return grpc::Status::OK;
}
