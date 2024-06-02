cmake_minimum_required(VERSION 3.10)
project(zippy)

set(CMAKE_CXX_STANDARD 17)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Include directories
include_directories(include)

# Add executable
add_executable(zippy ${SOURCES})

# Enable testing
enable_testing()
add_subdirectory(tests)
