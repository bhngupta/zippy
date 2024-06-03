#include "Server.h"
#include <iostream>

void Server::Run() {
    std::string server_address("0.0.0.0:50051");
    ZippyServiceImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    server = builder.BuildAndStart();
    std::cout << "Server started at " << server_address << std::endl;

    server->Wait();
}