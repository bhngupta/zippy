// Server.cpp
#include <iostream>
#include <string>
#include "Server.h"

void Server::Run() {
    std::string server_address("0.0.0.0:50051");
    Database db(3); // Create a Database instance

    ZippyService service(db); // Pass the Database instance to ZippyService

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart()); // Assign the server pointer
    std::cout << "Server listening on " << server_address << std::endl;

    server->Wait();
}
