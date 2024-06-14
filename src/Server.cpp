// Server.cpp
#include "Server.h"

#include <iostream>
#include <string>

void Server::Run() {
  std::string server_address("0.0.0.0:50051");
  Database db(10000, 600, std::chrono::minutes(2), "../snapshot.bin");

  // TODO- Check if this runs on different thread
  db.startBackgroundThread();

  // ZippyService starts server instance using DB
  ZippyService service(db);

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  service.cq_ = builder.AddCompletionQueue();
  service.server_ = builder.BuildAndStart();
  std::cout << "Server listening on " << server_address << std::endl;

  for (int i = 0; i < 3; i++) {
    service.threads_.emplace_back(&ZippyService::HandleRpcs, &service);
  }

  service.server_->Wait();
  db.stopBackgroundThread();
}
