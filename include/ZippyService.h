#ifndef ZIPPYSERVICEIMPL_H
#define ZIPPYSERVICEIMPL_H

#include "zippy.grpc.pb.h"
#include "Database.h"
#include <grpcpp/grpcpp.h>
#include <thread>
#include <vector>

class ZippyService final : public zippy::Zippy::AsyncService {
public:
    explicit ZippyService(Database& db);
    ~ZippyService();

    grpc::Status ExecuteCommand(grpc::ServerContext* context, const zippy::CommandRequest* request, zippy::CommandResponse* response) override;

    void HandleRpcs();

    void log(const std::string& operation, const std::string& key, const std::string& value = "");

    std::vector<std::thread> threads_;
    std::unique_ptr<grpc::ServerCompletionQueue> cq_;
    std::unique_ptr<grpc::Server> server_;

private:
    Database& db_;
};

#endif // ZIPPYSERVICEIMPL_H
