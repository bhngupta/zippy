#include "ZippyClient.h"
#include <grpcpp/grpcpp.h>
#include <iostream>

int main(int argc, char** argv) {
    ZippyClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    // Test SET command
    std::string setCommand = "SET test_key test_value";
    std::string reply = client.ExecuteCommand(setCommand);
    std::cout << "Set response: " << reply << std::endl;

    // Test GET command
    std::string getCommand = "GET test_key";
    reply = client.ExecuteCommand(getCommand);
    std::cout << "Get response: " << reply << std::endl;

    // Test DEL command
    std::string delCommand = "DEL test_key";
    reply = client.ExecuteCommand(delCommand);
    std::cout << "Del response: " << reply << std::endl;

    return 0;
}
