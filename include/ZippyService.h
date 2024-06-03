#ifndef ZIPPYSERVICE_H
#define ZIPPYSERVICE_H

#include <grpcpp/grpcpp.h>
#include "zippy.grpc.pb.h"
#include "Database.h"

class ZippyService final : public zippy::ZippyService::Service {
public:
    explicit ZippyService(Database& db) : db_(db) {}

    grpc::Status Set(grpc::ServerContext* context, const zippy::SetRequest* request,
                     zippy::SetResponse* response) override;

    grpc::Status Get(grpc::ServerContext* context, const zippy::GetRequest* request,
                     zippy::GetResponse* response) override;

    grpc::Status Del(grpc::ServerContext* context, const zippy::DelRequest* request,
                     zippy::DelResponse* response) override;

private:
    Database& db_;
};

#endif // ZIPPYSERVICE_H
