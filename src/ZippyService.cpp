#include "ZippyService.h"

grpc::Status ZippyServiceImpl::Set(grpc::ServerContext* context, const zippy::SetRequest* request, zippy::SetResponse* response) {
    db_.set(request->key(), request->value());
    response->set_status("OK");
    return grpc::Status::OK;
}

grpc::Status ZippyServiceImpl::Get(grpc::ServerContext* context, const zippy::GetRequest* request, zippy::GetResponse* response) {
    std::string value = db_.get(request->key());
    response->set_value(value);
    return grpc::Status::OK;
}

grpc::Status ZippyServiceImpl::Del(grpc::ServerContext* context, const zippy::DelRequest* request, zippy::DelResponse* response) {
    db_.del(request->key());
    response->set_status("OK");
    return grpc::Status::OK;
}
