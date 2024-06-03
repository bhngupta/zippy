#ifndef ZIPPY_SERVICE_H
#define ZIPPY_SERVICE_H

#include "zippy.grpc.pb.h"
#include "Database.h"
#include <grpcpp/grpcpp.h>

class ZippyServiceImpl final : public zippy::ZippyService::Service {
public:
    ZippyServiceImpl(Database& db) : db_(db) {}

    grpc::Status Set(grpc::ServerContext* context, const zippy::SetRequest* request, zippy::SetResponse* response) override;
    
    grpc::Status Get(grpc::ServerContext* context, const zippy::GetRequest* request, zippy::GetResponse* response) override;
    
    grpc::Status Del(grpc::ServerContext* context, const zippy::DelRequest* request, zippy::DelResponse* response) override;

private:
    Database& db_;
};

#endif // ZIPPY_SERVICE_H
