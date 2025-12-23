# This is the configuration file for this project. The settings here are used by
# build and test scripts. This file is not to be used directly and modified with
# care. Whenever building the project or running a script ensure you are always
# in the root directory.

import os
import platform
from pathlib import Path

import time

ROOT_DIR = Path(__file__).resolve().parent

# These variables are used to set the COMPILER and ADDITIONAL_FLAGS according to
# the platform. Change them according to the compilers you are using. For the below
# example it is assumed that the commands 'x86_64-w64-mingw32-gcc' and 'g++' work
# in the shell. If that is not the case then you need to provide the absolute path
# of the respective binaries.
WINDOWS_COMPILER = "x86_64-w64-mingw32-gcc"
WINDOWS_ADDITIONAL_FLAGS = ["-shared", "-static-libgcc"]
LINUX_COMPILER = "g++"
LINUX_ADDITIONAL_FLAGS = ["-fPIC", "-shared"]

VERSION = "v1.0.0"

OUTPUT_LIBRARY_NAME = f"{int(time.time())}-{platform.platform()}-dfs-core-{VERSION}"

# COMPILER and ADDITIONAL_FLAGS will be used by the build scripts
if platform.system() == "Windows":
    COMPILER = WINDOWS_COMPILER
    ADDITIONAL_FLAGS = WINDOWS_ADDITIONAL_FLAGS
    OUTPUT_LIBRARY_NAME += ".dll"
else:
    COMPILER = LINUX_COMPILER
    ADDITIONAL_FLAGS = LINUX_ADDITIONAL_FLAGS
    OUTPUT_LIBRARY_NAME += ".so"

BUILD_DIRECTORY = os.path.join(ROOT_DIR, "build")
DEBUG_DIRECTORY = os.path.join(ROOT_DIR, "debug")
SOURCE_DIRECTORY = os.path.join(ROOT_DIR, "src")
INCLUDE_DIRECTORY = os.path.join(ROOT_DIR, "include")
PROTOS_DIRECTORY = os.path.join(ROOT_DIR, "protos", "")
DFS_CLIENT_DIRECTORY = os.path.join(ROOT_DIR, "dfs_user_client", "")
DFS_STNODE_DIRECTORY = os.path.join(ROOT_DIR, "dfs_storage_node", "")
DFS_TSERVER_DIRECTORY = os.path.join(ROOT_DIR, "dfs_tracking_server", "")

# These are shell commands make sure to add the ones for your platform
# Used to remove .o files after each run of the script
RM_CMD = "rm"
# To copy
CP_CMD = "cp"

def filePathsInDirectory(directory: str) -> list:
    """ This function is used by the build scripts to find all the files in a given
        directory. You can modify this function if you wish to find files in a 
        different way. However, the function must accept a string (directory path)
        and must return a list of strings (file paths). """

    file_paths = []

    for root, directories, files in os.walk(directory):
        for filename in files:
            file_paths.append(os.path.join(root, filename))

    return file_paths