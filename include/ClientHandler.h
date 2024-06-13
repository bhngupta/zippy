// Classes and methods for ClientHandler Header
#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "zippy.grpc.pb.h"
#include "Database.h"
#include "ZippyService.h"
#include <grpcpp/grpcpp.h>

class ZippyService;

class ClientHandler {
public:
    ClientHandler(Database* db, zippy::Zippy::AsyncService* service, grpc::ServerCompletionQueue* cq);

    void Proceed();

private:
    Database* db_;
    zippy::Zippy::AsyncService* service_;
    grpc::ServerCompletionQueue* cq_;
    grpc::ServerContext context_;
    zippy::CommandRequest request_;
    zippy::CommandResponse response_;
    grpc::ServerAsyncResponseWriter<zippy::CommandResponse> responder_;

    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status_;
};

#endif // CLIENTHANDLER_H
