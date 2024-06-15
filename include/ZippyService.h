#ifndef ZIPPYSERVICEIMPL_H
#define ZIPPYSERVICEIMPL_H

#include "Database.h"
#include "zippy.grpc.pb.h"

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

class ZippyService final : public zippy::Zippy::AsyncService {
public:
  explicit ZippyService(Database &db);
  ~ZippyService();
  void Shutdown();
  grpc::Status ExecuteCommand(grpc::ServerContext *context, const zippy::CommandRequest *request,
                              zippy::CommandResponse *response) override;

  void HandleRpcs();
  void RemoveClientID(grpc::ServerContext *context);

  void log(const std::string &client_id, const std::string &operation, const std::string &key,
           const std::string &value = "");

  std::vector<std::thread> threads_;
  std::unique_ptr<grpc::ServerCompletionQueue> cq_;
  std::unique_ptr<grpc::Server> server_;

private:
  Database &db_;
  std::unordered_map<grpc::ServerContext *, std::string> client_ids_;
  std::mutex client_ids_mutex_;

  std::mutex shutdown_mutex_;
  std::condition_variable shutdown_cv_;
  std::atomic<bool> shutdown_;
  std::string getCurrentTime();
};

#endif
