#Zippy Key-Value Store
Zippy is a gRPC-based key-value store that supports expiration, eviction, and snapshot features. This project demonstrates the implementation of a basic key-value store using gRPC for communication between clients and the server.

Features
Set, Get, Delete Operations: Basic key-value operations with gRPC.
Expiration: Automatically delete keys after a fixed TTL (Time to Live).
Eviction: Use LRU (Least Recently Used) policy to evict keys when the capacity is exceeded.
Snapshots: Periodically save the state of the key-value store to a snapshot file.
Project Structure
include/: Header files for the project.
src/: Implementation files for the project.
gen/: Generated files from the protobuf compiler.
tests/: Unit tests for the project.
CMakeLists.txt: CMake build configuration.
Prerequisites
C++17 or later
CMake 3.10 or later
gRPC and Protobuf libraries
Google Test (gtest)