#ifndef ZIPPYCLIENT_H
#define ZIPPYCLIENT_H

#include <grpcpp/grpcpp.h>
#include "zippy.grpc.pb.h"

class ZippyClient {
public:
    ZippyClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(zippy::Zippy::NewStub(channel)) {}

    std::string ExecuteCommand(const std::string& command) {
        zippy::CommandRequest request;
        request.set_command(command);

        zippy::CommandResponse response;
        grpc::ClientContext context;

        grpc::Status status = stub_->ExecuteCommand(&context, request, &response);
        if (status.ok()) {
            return response.result();
        } else {
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<zippy::Zippy::Stub> stub_;
};

#endif // ZIPPYCLIENT_H
