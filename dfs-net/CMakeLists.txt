cmake_minimum_required(VERSION 3.10)
project(dfs-net)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Example grpc plugin path: /home/saksham/grpc_install_dir/bin/grpc_cpp_plugin
if (NOT DEFINED ENV{DFSN_GRPC_CPP_PLUGIN})
  message(FATAL_ERROR "Environment variable DFSN_GRPC_CPP_PLUGIN is not set. Please set this to your GRPC C++ Plugin Path.")
endif()

set(DFSN_GRPC_CPP_PLUGIN_EXECUTABLE $ENV{DFSN_GRPC_CPP_PLUGIN})

# gRPC and Protobuf
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../proto/chunkcomm.proto)

add_library(protolib STATIC)

protobuf_generate(
  LANGUAGE cpp
  OUT_VAR PROTO_SRCS
  TARGET protolib
  IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../proto
  PROTOS ${PROTO_FILES}
)

protobuf_generate(
  LANGUAGE grpc
  OUT_VAR GRPC_SRCS
  TARGET protolib
  GENERATE_EXTENSIONS .grpc.pb.hh .grpc.pb.cc
  PLUGIN "protoc-gen-grpc=${DFSN_GRPC_CPP_PLUGIN_EXECUTABLE}"
  IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../proto
  PROTOS ${PROTO_FILES}
)

target_sources(protolib PRIVATE ${PROTO_SRCS} ${GRPC_SRCS})
target_link_libraries(protolib gRPC::grpc++ protobuf::libprotobuf)
target_include_directories(protolib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# libsodium
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)
include_directories(${LIBSODIUM_INCLUDE_DIRS})

set(DFS_NET_SOURCES
  chunk_ingestor.cc
  chunk_transmitter.cc
)

add_library(dfs-net ${DFS_NET_SOURCES})
target_include_directories(dfs-net
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(dfs-net ${LIBSODIUM_LIBRARIES} protolib gRPC::grpc++ protobuf::libprotobuf dfs-core)

add_executable(test_client test_client.cc)
add_executable(test_server test_server.cc)

target_link_libraries(test_client dfs-net)
target_link_libraries(test_server dfs-net)