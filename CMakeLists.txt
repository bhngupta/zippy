cmake_minimum_required(VERSION 3.10)
project(zippy)

set(CMAKE_CXX_STANDARD 17)

# Find gRPC and Protobuf packages
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

# List all source files
set(SOURCES
    src/Database.cpp
    src/ClientHandler.cpp
    src/HashTable.cpp
    src/Server.cpp
    src/ZippyService.cpp
    gen/zippy.pb.cc
    gen/zippy.grpc.pb.cc
    # [IMP] - DO NOT ADD main.cpp HERE IT MESSES UP THE TESTS
    # Add other source files here
)

# List all header files
set(HEADERS
    include/Database.h
    include/ClientHandler.h
    include/HashTable.h
    include/Server.h
    include/ZippyService.h
    include/zippy.pb.h
    include/zippy.grpc.pb.h
    # Add other header files here
)

# Include directories
include_directories(include/)

# Add executable for the main application
add_executable(zippy ${SOURCES} src/main.cpp)

# Link gRPC and Protobuf libraries
target_link_libraries(zippy gRPC::grpc++ protobuf::libprotobuf)

message(STATUS "Building Tests")

# Include Google Test
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

set(TEST_SOURCES
    tests/test.cpp
    tests/test_main.cpp # Create a separate main for tests
)

# Add test executable
add_executable(tests ${TEST_SOURCES} ${SOURCES})

# Link against Google Test
target_link_libraries(tests gtest_main)

# Add a custom target to run tests
add_custom_target(run_tests
    COMMAND tests --gtest_color=yes
    DEPENDS tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
