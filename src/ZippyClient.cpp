#include "ZippyClient.h"
#include <grpcpp/grpcpp.h>
#include <iostream>

void printMenu() {
    std::cout << "\nChoose an operation:\n";
    std::cout << "1. SET a value\n";
    std::cout << "2. GET a value\n";
    std::cout << "3. DEL a value\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter your choice: ";
}

int main(int argc, char** argv) {
    ZippyClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    int choice;
    std::string key, value, command, reply;

    do {
        printMenu();
        std::cin >> choice;

        switch (choice) {
            case 1:  // SET
                std::cout << "Enter key: ";
                std::cin >> key;
                std::cout << "Enter value: ";
                std::cin >> value;
                command = "SET " + key + " " + value;
                reply = client.ExecuteCommand(command);
                std::cout << "Set response: " << reply << std::endl;
                break;

            case 2:  // GET
                std::cout << "Enter key: ";
                std::cin >> key;
                command = "GET " + key;
                reply = client.ExecuteCommand(command);
                std::cout << "Get response: " << reply << std::endl;
                break;

            case 3:  // DEL
                std::cout << "Enter key: ";
                std::cin >> key;
                command = "DEL " + key;
                reply = client.ExecuteCommand(command);
                std::cout << "Del response: " << reply << std::endl;
                break;

            case 4:  // Exit
                std::cout << "Exiting...\n";
                break;

            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != 4);

    return 0;
}
