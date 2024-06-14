# Zippy

Zippy is a lightweight, gRPC-based in-memory data store that offers robust key-value storage capabilities, akin to Redis. Engineered for high performance, Zippy excels at handling multiple operations concurrently, making it an ideal solution for applications demanding efficient, real-time data storage and retrieval.

## Features

- Data Management: Efficiently add, update, and delete data with seamless CRUD operations.
- Data Persistence: Data durability through regular merge-on-update snapshotting, providing reliable data recovery.
- Data Expiration: Removes entries after a specified time-to-live (TTL).
- Data Eviction: Manages memory efficiently by evicting less frequently used data.
- Concurrency: Handles multiple clients simultaneously with robust multi-threading and concurrent processing.

## The WHY?

This project was born from a question discussed over the dinner table — "How do companies like Uber and WhatsApp manage millions of data transactions every second?". Intrigued by the complexity of this challenge, we delved into the inner workings of high-performance systems, inspired by this [Uber Engineering Blog](https://www.uber.com/blog/how-uber-serves-over-40-million-reads-per-second-using-an-integrated-cache/). We got interested in the inner workings of Redis and wanted to learn how it works. What better way to learn and master the art of system design by implementing it by ourselves. 


## Choice of Language

Zippy is crafted for speed, reliability, and scalability, making it the optimal solution for applications demanding rapid data access and manipulation.

We prioritized ultra-low latency and high performance for our data store. Selecting C++ as our primary backend language was driven by the following advantages:

- Extremely fast
- Low level control of the memory
- Flexibility to design data structure

This choice equips Zippy with the capabilities needed to handle intensive workloads efficiently, ensuring swift and reliable data operations.

## Components of Zippy

Our system has 2 main parts:

1. Database: This is where we store the data. Think of it like a bed-side table (Storage for books for which you require fast access).
2. Registry: This is where we store data for longer. Think of it like a book shelf (Storage for books for which you don't require fast access).

## Architecture

![Zippy Architecture Diagram](https://github.com/bhngupta/zippy/blob/main/misc/zippy-arch.png?raw=true)

## Getting Started

### Prerequisites

Ensure you have the following installed:

1. **Git** : Version control
2. **C++ Compiler** : For compiling C++ code
3. **CMake** : To build the project
4. **gRPC** : Simplifies and accelerates inter-program communication
5. **protobuf** : Serialization framework for data interchange

### Installation

0. Fork the repository

1. Clone the repository:

   ```bash
   git clone https://github.com/your_username/zippy
   cd zippy
   ```

2. (Optional) Run Hooks & Tests

   ```bash
   ./scripts/setup_hooks.sh
   ./scripts/run_tests.sh
   ```


3. Build the project:

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. Run the server:

   ```bash
   ./zippy
   ```

This will start the Zippy server

### Usage 

From the root of the project

1. Into Client directory

   ```bash
   cd client
   ```

2. Install Python dependency ([virtual environment](https://docs.python.org/3/library/venv.html))

   ```bash
   pip install -r requirements.txt
   ```

3. Start Pyhton Client

   ```bash
   python3 client.py
   ```

You can now communicate with the C++ server and use the in-memory datastore!

## Future Plans

- [x] ~~Implement snapshotting (saving the in-memory data to disk at intervals)~~
- [x] ~~Handle multiple clients concurrently using multi-threading or an event loop~~ 
- [ ] Make the tests run faster - tests/tDatabase.cpp 
- [ ] Simple Pub/Sub messaging 
- [ ] Support for multiple data structures
- [ ] Make a web client in flask to monitor the current status of the cache


## License

This project is licensed under the [MIT License](https://opensource.org/license/MIT) - see the [LICENSE](https://github.com/bhngupta/zippy/blob/main/LICENSE) file for details.
