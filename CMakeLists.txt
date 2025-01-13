# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(DecentralizedFileStorageNetwork)

add_subdirectory(client-core)
add_subdirectory(storage-node-core)
add_subdirectory(tracking-server-core)