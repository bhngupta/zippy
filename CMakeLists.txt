cmake_minimum_required(VERSION 3.10)
project(zippy)

set(CMAKE_CXX_STANDARD 17)

# List all source files
set(SOURCES
    src/Database.cpp
    src/ClientHandler.cpp
    src/HashTable.cpp
    src/Server.cpp
    main.cpp
    # Add other source files here
)

# List all header files
set(HEADERS
    include/Database.h
    include/ClientHandler.h
    include/HashTable.h
    include/Server.h
    # Add other header files here
)

# Include directories
include_directories(include/)

# Add executable for the main application
add_executable(zippy ${SOURCES} main.cpp)

message(STATUS "Building Tests")

# Include Google Test
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Add test executable
add_executable(tests tests/test.cpp ${SOURCES})

# Link against Google Test
target_link_libraries(tests gtest_main)

# Custom target to run tests
add_custom_target(run_tests
    COMMAND ./tests --verbose
    DEPENDS tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
