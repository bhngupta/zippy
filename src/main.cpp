#include "Server.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

// Custom stream buffer to prepend timestamps to log messages
class TimestampedBuffer : public std::streambuf {
public:
    TimestampedBuffer(std::streambuf* buf) : buf_(buf) {}

protected:
    virtual int overflow(int c) override {
        if (c != EOF) {
            if (newline_) {
                // Prepend timestamp at the beginning of a new line
                std::ostream os(buf_);
                os << "[" << getCurrentTime() << "] ";
                newline_ = false;
            }
            if (c == '\n') {
                newline_ = true;
            }
        }
        return buf_->sputc(c);
    }

    virtual int sync() override {
        return buf_->pubsync();
    }

private:
    std::streambuf* buf_;
    bool newline_ = true;
};

int main(int argc, char **argv) {
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
        // Create the logs directory if it doesn't exist
        std::filesystem::create_directories("logs");
        log_file.open(log_file_path, std::ios::out | std::ios::app);
        if (!log_file.is_open()) {
            std::cerr << "Failed to open log file: " << log_file_path << std::endl;
            return 1;
        }
        // Redirect std::cout to log file with timestamps
        std::cout.rdbuf(new TimestampedBuffer(log_file.rdbuf()));
    } else if (!log_to_terminal) {
        // Redirect std::cout to null if no logging is desired
        std::cout.setstate(std::ios_base::failbit);
    } else {
        // Redirect std::cout to terminal with timestamps
        std::cout.rdbuf(new TimestampedBuffer(std::cout.rdbuf()));
    }

  Server server;
  server.Run();

  // TODO - Will this ever run?
  if (log_to_file) {
    log_file.close();
  }

  return 0;
}