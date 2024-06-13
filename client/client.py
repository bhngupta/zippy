import grpc
import zippy_pb2
import zippy_pb2_grpc

class ZippyClient:
    def __init__(self, address='localhost:50051'):
        self.channel = grpc.insecure_channel(address)
        self.stub = zippy_pb2_grpc.ZippyStub(self.channel)

    def execute_command(self, command):
        request = zippy_pb2.CommandRequest(command=command)
        response = self.stub.ExecuteCommand(request)
        return response.result

if __name__ == "__main__":
    print('.....Starting Zippy Client.....')
    client = ZippyClient()
    print("Zippy Client. Type 'exit' or 'quit' to exit.")
    while True:
        try:
            command = input("Enter command (SET key value | GET key | DEL key): ").strip()
            if command.lower() in ["exit", "quit"]:
                break
            response = client.execute_command(command)
            print(f"Response: {response}")
        except Exception as e:
            print(f"Error: {e}")
