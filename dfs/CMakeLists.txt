cmake_minimum_required(VERSION 3.10)
project(decentralized_file_storage_network)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Find Protobuf and gRPC
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
  PLUGIN "protoc-gen-grpc=/home/saksham/grpc_install_dir/bin/grpc_cpp_plugin"
  IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../proto
  PROTOS ${PROTO_FILES}
)

target_sources(protolib PRIVATE ${PROTO_SRCS} ${GRPC_SRCS})
target_link_libraries(protolib gRPC::grpc++ protobuf::libprotobuf)

target_include_directories(protolib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Find libsodium
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)

include_directories(${LIBSODIUM_INCLUDE_DIRS})

# The actual dfs library
set(DFS_SOURCES
  src/stream_encrypt_decrypt.cc
  src/sequential_file_splitter.cc
  src/util.cc
  src/thread_pool.cc
  src/diagnostics.cc
  src/logger.cc
  src/chunk_transmitter.cc
)

add_library(dfs ${DFS_SOURCES})
target_include_directories(dfs PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(dfs ${LIBSODIUM_LIBRARIES} protolib gRPC::grpc++ protobuf::libprotobuf)

# executable
add_executable(test test/test.cc)
add_executable(test_reconst_unenc test/test_file_reconst_unenc.cc)
add_executable(test_endian test/test_endian.cc)
add_executable(test_log test/test_log.cc)
add_executable(test_new_enc_dec test/test_new_enc_dec.cc)
add_executable(test_split_reconst test/test_split_reconst.cc)
target_link_libraries(test dfs)
target_link_libraries(test_reconst_unenc dfs)
target_link_libraries(test_log dfs)
target_link_libraries(test_new_enc_dec dfs)
target_link_libraries(test_split_reconst dfs)