#include "ClientHandler.h"

#include "ZippyService.h"

#include <iostream>
#include <sstream>

ClientHandler::ClientHandler(Database *db, zippy::Zippy::AsyncService *service,
                             grpc::ServerCompletionQueue *cq)
    : db_(db), service_(service), cq_(cq), responder_(&context_), status_(CREATE) {
  Proceed();
}

void ClientHandler::Proceed() {
  if (status_ == CREATE) {
    status_ = PROCESS;
    service_->RequestExecuteCommand(&context_, &request_, &responder_, cq_, cq_, this);
  } else if (status_ == PROCESS) {
    new ClientHandler(db_, service_, cq_);

    std::string command = request_.command();
    std::istringstream iss(command);
    std::string arg;
    iss >> arg;

    ZippyService *zippy_service = static_cast<ZippyService *>(service_);

    // Get the client ID from the request
    std::string client_id = request_.client_id();

    if (arg == "SET") {
      std::string key, value;
      iss >> key >> value;
      db_->set(key, value);
      response_.set_result("Set operation performed successfully");
      zippy_service->log(client_id, arg, key, value);
    } else if (arg == "GET") {
      std::string key;
      iss >> key;
      std::string retrievedValue = db_->get(key);
      response_.set_result("Retrieved value: " + retrievedValue);
      zippy_service->log(client_id, arg, key, retrievedValue);
    } else if (arg == "DEL") {
      std::string key;
      iss >> key;
      db_->del(key);
      response_.set_result("Delete operation performed successfully");
      zippy_service->log(client_id, arg, key);
    } else {
      response_.set_result("Invalid command");
    }

    status_ = FINISH;
    responder_.Finish(response_, grpc::Status::OK, this);
  } else {
    ZippyService *zippy_service = static_cast<ZippyService *>(service_);

    // Removes ClientID when client closed
    zippy_service->RemoveClientID(&context_);
    GPR_ASSERT(status_ == FINISH);
    delete this;
  }
}
