#include <iostream>
#include "Server.h"

int main(int argc, char** argv) {
    bool log_to_terminal = false;
    bool log_to_file = false;
    std::string log_file_path;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-t") == 0) {
            log_to_terminal = true;
        } else if (std::strcmp(argv[i], "-n") == 0) {
            log_to_terminal = false;
        } else if (std::strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            log_to_file = true;
            log_file_path = argv[++i];
        }
    }

    std::ofstream log_file;
    if (log_to_file) {
        log_file.open(log_file_path, std::ios::out | std::ios::app);
        if (!log_file.is_open()) {
            std::cerr << "Failed to open log file: " << log_file_path << std::endl;
            return 1;
        }
        std::cout.rdbuf(log_file.rdbuf()); // Redirect std::cout to log file
    } else if (!log_to_terminal) {
        // Redirect std::cout to null if no logging is desired
        std::cout.setstate(std::ios_base::failbit);
    }

    Server server;
    server.Run();

    if (log_to_file) {
        log_file.close();
    }

    return 0;
}