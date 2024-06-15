import grpc
import filetransfer_pb2
import filetransfer_pb2_grpc
import os
import filesplit
import hashlib

DUMMY_TOKEN = "<OWNERSHIP-TOKEN>"

def run() -> int | None:
    print(f"[ INFO ] Splitting input file ...")

    file_was_split = filesplit.divideFileIntoChunks_unencrypted(
        "client_storage/text1.txt", 5, DUMMY_TOKEN,
        len(DUMMY_TOKEN), "client_storage/chunks/"
    )

    # if not file_was_split:
    #     print("[ ERROR ] File could not be split")
    #     return None

    file_paths = []

    # Detect the file chunks
    for root, directories, files in os.walk("client_storage/chunks/client_storage"):
        for filename in files:
            file_paths.append(os.path.join(root, filename))

    if len(file_paths) == 0:
        return None

    with grpc.insecure_channel("localhost:50051") as channel:
        stub = filetransfer_pb2_grpc.SimpleFileUploadServiceStub(channel)

        for file_path in file_paths:

            with open(file_path, 'rb') as f:
                data = f.read()
                calculatedHash = hashlib.sha512(data).hexdigest()

            response = stub.simpleUpload(filetransfer_pb2.simpleUploadRequest(
                clientCalculatedHash=calculatedHash, fileData=data
            ))

            print(f"[ {response.statusCode} ] SERVER -> {response.detail}")

if __name__ == "__main__":
    run()
        