cmake_minimum_required(VERSION 3.10)
project(storage_node_core)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../proto/chunkcomm.proto)
add_library(protolibsnc STATIC)

protobuf_generate(
  LANGUAGE cpp
  OUT_VAR PROTO_SRCS
  TARGET protolibsnc
  IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/ ../proto
  PROTOS ${PROTO_FILES}
)

protobuf_generate(
  LANGUAGE grpc
  OUT_VAR GRPC_SRCS
  TARGET protolibsnc
  GENERATE_EXTENSIONS .grpc.pb.hh .grpc.pb.cc
  PLUGIN "protoc-gen-grpc=/home/saksham/grpc_install_dir/bin/grpc_cpp_plugin"
  IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../proto
  PROTOS ${PROTO_FILES}
)

target_sources(protolibsnc PRIVATE ${PROTO_SRCS} ${GRPC_SRCS})
target_link_libraries(protolibsnc gRPC::grpc++ protobuf::libprotobuf)
target_include_directories(protolibsnc PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)
include_directories(${LIBSODIUM_INCLUDE_DIRS})

add_executable(storage_node_core src/main.cc)
target_link_libraries(storage_node_core dfs protolibsnc ${LIBSODIUM_LIBRARIES})
target_include_directories(storage_node_core PRIVATE ${CMAKE_SOURCE_DIR}/dfs/include)