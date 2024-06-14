import time
import threading
import grpc
import zippy_pb2
import zippy_pb2_grpc
from concurrent.futures import ThreadPoolExecutor

class ZippyClient:
    def __init__(self, address='localhost:50051'):
        self.channel = grpc.insecure_channel(address)
        self.stub = zippy_pb2_grpc.ZippyStub(self.channel)
        self.client_id = "benchmark-client"

    def execute_command(self, command):
        request = zippy_pb2.CommandRequest(command=command, client_id=self.client_id)
        response = self.stub.ExecuteCommand(request)
        return response.result

def benchmark(client, operation, num_requests, results, index):
    times = []
    for _ in range(num_requests):
        start_time = time.time()
        if operation == "SET":
            client.execute_command(f"SET key{_} value{_}")
        elif operation == "GET":
            client.execute_command(f"GET key{_}")
        elif operation == "DEL":
            client.execute_command(f"DEL key{_}")
        end_time = time.time()
        times.append(end_time - start_time)
    total_time = sum(times)
    average_time = total_time / num_requests
    throughput = num_requests / total_time
    results[index] = (total_time, throughput, average_time)

def run_benchmark(num_clients=10, num_requests=1000):
    clients = [ZippyClient() for _ in range(num_clients)]
    results = {}

    wall_clock_start = time.time()

    with ThreadPoolExecutor(max_workers=num_clients) as executor:
        for operation in ["SET", "GET", "DEL"]:
            operation_results = [None] * num_clients
            futures = [
                executor.submit(benchmark, clients[i], operation, num_requests, operation_results, i)
                for i in range(num_clients)
            ]
            for future in futures:
                future.result()  # wait for all threads to complete

            total_time = sum(result[0] for result in operation_results)
            total_throughput = sum(result[1] for result in operation_results)
            average_time = sum(result[2] for result in operation_results) / num_clients
            average_throughput = total_throughput / num_clients
            results[operation] = (total_time, average_throughput, average_time)

    wall_clock_end = time.time()
    wall_clock_time = wall_clock_end - wall_clock_start

    print("Zippy Benchmark Results:\n")
    print(f"{'Operation':<10} {'Avg Throughput (ops/sec)':<25} {'Avg Time per Op (seconds)':<25}")
    print("="*60)
    for operation, (total_time, avg_throughput, avg_time) in results.items():
        print(f"{operation:<10} {avg_throughput:<25.2f} {avg_time:<25.6f}")
    print("="*60)
    print(f"\nWall-Clock Time: {wall_clock_time:.2f} seconds\n")

if __name__ == "__main__":
    num_clients = 10
    num_requests = 1000

    run_benchmark(num_clients, num_requests)