#ifndef ZIPPYSERVICEIMPL_H
#define ZIPPYSERVICEIMPL_H

#include "zippy.grpc.pb.h"
#include "Database.h"

class ZippyService final : public zippy::Zippy::Service {
public:
    explicit ZippyService(Database& db) : db_(db) {}

    grpc::Status ExecuteCommand(grpc::ServerContext* context, const zippy::CommandRequest* request, zippy::CommandResponse* response) override;

private:
    Database& db_;
};

#endif // ZIPPYSERVICEIMPL_H
