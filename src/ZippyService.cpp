#include "ZippyService.h"

grpc::Status ZippyService::Set(grpc::ServerContext* context, const zippy::SetRequest* request,
                               zippy::SetResponse* response) {
    db_.set(request->key(), request->value());
    response->set_success(true);
    return grpc::Status::OK;
}

grpc::Status ZippyService::Get(grpc::ServerContext* context, const zippy::GetRequest* request,
                               zippy::GetResponse* response) {
    std::string value = db_.get(request->key());
    response->set_value(value);
    return grpc::Status::OK;
}

grpc::Status ZippyService::Del(grpc::ServerContext* context, const zippy::DelRequest* request,
                               zippy::DelResponse* response) {
    db_.del(request->key());
    response->set_success(true);
    return grpc::Status::OK;
}
