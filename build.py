# This is NOT A BUILD SYSTEM. This is a simple script to automate the build process
# and cannot detect success or failure of the build.

# NOTE: Everything inside the SOURCE_DIRECTORY is considered a part of dfs core

import os
import sys

from _project_config import *

args = sys.argv

# For people like me who compile for both windows and linux in WSL itself
# Not recommended to use unless you really need to compile for windows
if "--windows" in args:
    # Manually override
    COMPILER = WINDOWS_COMPILER
    ADDITIONAL_FLAGS = WINDOWS_ADDITIONAL_FLAGS
    OUTPUT_LIBRARY_NAME = f"windows-target-dfs-core-{VERSION}.dll"

if (("--debug" in args) and ("--dfs-core" in args)):
    print(f"[ INFO ] Starting debug build ...")
    print(f"[ INFO ] Getting source paths ...")

    src_file_paths = filePathsInDirectory(SOURCE_DIRECTORY)
    object_files = []

    print(f"[ INFO ] Building objects ...")

    for src_file in src_file_paths:

        command = f"{COMPILER} -I\"{INCLUDE_DIRECTORY}\" -c {src_file} -o {src_file}.o"
        print(f"[ INFO ] Run -> {command}")
        os.system(command)
        object_files.append(f"{src_file}.o")

    print(f"[ INFO ] Building {OUTPUT_LIBRARY_NAME} ...")

    command = f"{COMPILER} {' '.join(ADDITIONAL_FLAGS)} -o {os.path.join(DEBUG_DIRECTORY, OUTPUT_LIBRARY_NAME)} {' '.join(object_files)}"
    
    print(f"[ INFO ] Run -> {command}")
    os.system(command)

    if "--preserve-obj" not in args:
        print(f"[ INFO ] Removing object files ...")

        for object_file in object_files:
            command = f"{RM_CMD} {object_file}"
            print(f"[ INFO ] Run -> {command}")

            os.system(command)

    print("Script for --debug and --dfs-core finished")

if "--storage-client-grpc":
    # Generating _pb2.py and _pb2_grpc.py via protoc

    # Hard coded for now
    # command = f"python3 -m grpc_tools.protoc -I\"protos/\" --python_out=\"dfs_storage_node/\" --pyi_out=\"dfs_storage_node/\" --grpc_out=\"dfs_storage_node/\" {args[args.index('--storage-client-grpc')+1]}"
    hard_coded_command = r'python3 -m grpc_tools.protoc -I"protos/" --python_out="dfs_storage_node/" --pyi_out="dfs_storage_node/" --grpc_python_out="dfs_storage_node/" protos/filetransfer.proto'
    print(f"[ INFO ] Run -> {hard_coded_command}")

    os.system(hard_coded_command)

    files_in_stnode = filePathsInDirectory(DFS_STNODE_DIRECTORY)
    # Copying generated files into client directory

    print(f"[ INFO ] Copying files into {DFS_CLIENT_DIRECTORY} ...")
    for filename in files_in_stnode:
        if "pb2" in filename:
            command = f"{CP_CMD} {filename} {DFS_CLIENT_DIRECTORY + filename.split('/')[-1]}"
            print(f"[ INFO ] Run -> {command}")
            os.system(command)

    print("Script for --storage-client-grpc finished")




# Compile for linux:  gcc -fPIC -shared -o ./dlls/fileops_tests.so fileops_tests.c

# BDLL_CMD = r"x86_64-w64-mingw32-gcc -shared -static-libgcc -o ./dlls/fileops_tests.dll fileops_tests.c"
# PYTN_SPT = r"cmd.exe /c python tests_c_fileops_tests.py"



# if "--builddll" in args:
#     print(f"[ INFO ] Building dll. Running command: \n{BDLL_CMD}")
#     os.system(BDLL_CMD)

# if "--noscript" not in args:
#     print(f"[ INFO ] Starting python script to load dll. Running command: \n{PYTN_SPT}")
#     os.system(PYTN_SPT)