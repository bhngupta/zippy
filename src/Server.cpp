// Server.cpp
#include <iostream>
#include <string>
#include "Server.h"

void Server::Run() {
    std::string server_address("0.0.0.0:50051");
    Database db(10, 600, std::chrono::minutes(2));

    db.startBackgroundThread();

    ZippyService service(db); // Pass the Database instance to ZippyService

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    service.cq_ = builder.AddCompletionQueue();
    service.server_ = builder.BuildAndStart();
    // std::unique_ptr<grpc::Server> server(builder.BuildAndStart()); // Assign the server pointer
    std::cout << "Server listening on " << server_address << std::endl;

    for (int i = 0; i < 3; i++) {
        service.threads_.emplace_back(&ZippyService::HandleRpcs, &service);
    }

    service.server_->Wait();
    db.stopBackgroundThread(); 
}
