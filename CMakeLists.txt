# Root CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(DecentralizedFileStorageNetwork)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(dfs)
add_subdirectory(storage-node-core)
add_subdirectory(tracking-server-core)
add_subdirectory(demo-server)
add_subdirectory(demo-client)