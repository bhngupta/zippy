// Classes and methods for Server Header
#ifndef SERVER_H
#define SERVER_H

#include <grpcpp/grpcpp.h>
#include "ZippyServiceImpl.h"

class Server {
public:
    
    void Run();

private:
    std::unique_ptr<grpc::Server> server;
};

#endif // SERVER_H
