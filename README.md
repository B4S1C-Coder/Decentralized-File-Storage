# Decentralized File Storage Network (DFS)
DFS is a small distributed file storage network which allows the user to split and disperse their files over a network ensuring that other than the user no one else can re-construct the files. The network is based on gRPC and peer-to-peer communication, there is no middle man between the user and the storage node.

## Techstack
- [Django](https://www.djangoproject.com/)
- [Django REST Framework](https://www.django-rest-framework.org/)
- [gRPC](https://grpc.io/)
- [protobuf](https://protobuf.dev/)
- [MySQL](https://www.mysql.com/)
- [SQLite](https://sqlite.org/)

The diagram below shows a detailed working of DFS.

![Working](https://github.com/B4S1C-Coder/Decentralized-File-Storage/blob/main/DecentralizedFileStorage.jpg)

>**Note**: As of now the project is under development and the first prototype will be implemented in python and C/C++ after that everything will likely be converted to C/C++ (or any other compiled language).

## Targets for First Prototype

- **Storage service for less conscious users**: Normally the information regarding which chunks are stored on which node is kept strictly on the client machine to adhere to the main goal of the project. However, if the client wishes they can store this information on a tracking server (**this is optional**) so that when they change machines they do not have to manually import the data (previously mentioned) to the new machine.

- Create a CLI and a GUI (desktop)

## Setup
This project involves a build step. There are two ways you can set up this project.

1. Clone the repo via `git clone https://github.com/B4S1C-Coder/Decentralized-File-Storage`

2. Create a virtual environment via `virtualenv .proto-env` (the name ".proto-env" is already included in `.gitignore`, you can change the name according to your needs)

3. Activate the enviornment and install dependencies:
  - `source .proto-env/bin/activate` for Linux and `.proto-env/bin/activate.bat` for Windows
  - `pip3 install -r requirements.txt` replace "pip3" with "pip" for Windows

4. While the repo includes pre-built binaries for Windows and Linux, it is highly recommended you build them from source. For Linux there is a build script `build.py` that will automatically build the `.so` files and the `_pb2.py`, `_pb2_grpc.py` needed for gRPC. However, the script is not as robust and you may need to build manually in case it fails.

### Manual Way
5. Build the binaries (assuming `g++`) (in one line):
  - For Linux: `g++ -shared -I "include/" -o "debug/dfs-core.so" -fPIC src/*.c`
  - For Windows: `g++ -shared -static-libgcc -I "include/" -o "debug/dfs-core.dll" src/*.c`

  >**Note**: For Windows you can omit the `-static-libgcc` flag if you have `msvcrt.dll`, basically you can omit this flag if your compiled C/C++ code does not raise the error "Can't find msvcrt.dll" on execution. But on a safer side you can use this flag but this increases the size of the final binary.

6. Generate `_pb2.py`, `_pb2_grpc.py` for gRPC:
  - `python3 -m grpc_tools.protoc -I"protos/" --python_out="dfs_storage_node/" --pyi_out="dfs_storage_node/" --grpc_python_out="dfs_storage_node/" protos/filetransfer.proto` replace "python3" with "python" for Windows.
  - The files would be generated in `dfs_storage_node/` **copy** them to `dfs_user_client/` since the client would also need those files in order to communicate with the storage node.

7. Edit `dfs_user_client/filesplit.py`, set `so_dll_path = os.path.join(ROOT_DIR, "debug", r"<Name of the output binary generated in step 5>")`. It is better to use absolute paths whenever importing a DLL / SO (these must not have name mangling).

>**Note**: If you get `undefined reference to: symbol` in `dfs_user_client/filesplit.py` at runtime, there is a high chance that there is name mangling in you .so / .dll. To check the exported symbols of the .so use `nm debug/<your binary name>.so`, this will show you the exported symbols. If you see symobols such as `_Z32divideFileIntoChunks_unencryptedPKciS0_iS0_` it means there is name mangling in your binary. In this case you can either recompile the binary and ensure that the header files have `extern "C" {DFS_EXPORT int functionName(...);}` OR inside `dfs_user_client/filesplit.py` you can replace `divideFileIntoChunks_unencrypted` with `_Z32divideFileIntoChunks_unencryptedPKciS0_iS0_`.

### Build Script Way
5. To build the binaries and the grpc files: `python3 build.py --debug --dfs-core --storage-client-grpc protos/filetransfer.proto`
  - To build only the binaries only use `--debug` and `--dfs-core`
  - To build only the grpc use `--storage-client-grpc` and `protos/filetransfer.proto`

If the build script executes correctly, thats it!

8. `mkdir dfs_storage_node/data` then `mkdir dfs_user_client/client_storage` then `mkdir dfs_user_client/client_storage/chunks` then `mkdir dfs_user_client/client_storage/chunks/client_storage` put the payload (to send to storage node) in `dfs_user_client/client_storage/`

>**Note**: The script is only tested on Linux (WSL), if you are on Windows either risk it or just use WSL. The last step is just some directories according to the present code, you can change it to your liking.

## Running the Project
1. Activate the virtual environment.

2. `cd dfs_storage_node/` then `python3 storage_node.py`

3. In a different terminal repeat step 1, then `cd dfs_user_client/` then `python3 user_client.py`