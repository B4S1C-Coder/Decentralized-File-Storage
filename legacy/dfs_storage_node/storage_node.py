from concurrent import futures

import grpc
import filetransfer_pb2
import filetransfer_pb2_grpc

import hashlib
import logging

class SimpleFileUploadService(filetransfer_pb2_grpc.SimpleFileUploadServiceServicer):
    def simpleUpload(
        self, request: filetransfer_pb2.simpleUploadRequest, context
        ) -> filetransfer_pb2.simpleUploadReply:

        data: bytes = request.fileData
        serverCalulatedHash: str = hashlib.sha512(data).hexdigest()

        if request.clientCalculatedHash != serverCalulatedHash:
            return filetransfer_pb2.simpleUploadReply(
                statusCode=400, detail="Hashes do not match"
            )

        try:
            with open(f"data/{request.clientCalculatedHash}.dat", "wb") as f:
                f.write(data)
                return filetransfer_pb2.simpleUploadReply(
                    statusCode=201, detail=f"Uploaded {len(data)} bytes"
                )
        except Exception as err:
            return filetransfer_pb2.simpleUploadReply(
                statusCode=401, detail=err
            )

def serve():
    PORT = 50051
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))

    filetransfer_pb2_grpc.add_SimpleFileUploadServiceServicer_to_server(SimpleFileUploadService(), server)

    server.add_insecure_port(f"[::]:{PORT}")
    server.start()

    print(f"Server is listenting on port: {PORT}")

    try:
        server.wait_for_termination()
    except KeyboardInterrupt:
        print("Manual intervention shutting down server ...")
        server.stop(5)
        print("Server stopped")
    except Exception as err:
        print(f"Error occured:\n[ ERROR ] {err}\nShutting down server ...")
        server.stop(5)
        print("Server stopped")

if __name__ == "__main__":
    logging.basicConfig()
    serve()
