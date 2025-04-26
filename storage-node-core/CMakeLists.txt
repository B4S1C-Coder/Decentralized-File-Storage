cmake_minimum_required(VERSION 3.10)
project(storage_node_core)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# === Dependencies ===
find_package(SQLite3 REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)

# === Proto Library ===
set(PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../proto/chunkcomm.proto)

add_library(protolibsnc STATIC)

protobuf_generate(
  LANGUAGE cpp
  OUT_VAR PROTO_SRCS
  TARGET protolibsnc
  IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../proto
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

target_link_libraries(protolibsnc
  PUBLIC
    gRPC::grpc++
    protobuf::libprotobuf
)

target_include_directories(protolibsnc PUBLIC ${CMAKE_CURRENT_BINARY_DIR})


# === Core Library ===
set(CORELIB_SOURCES
  src/core/config.cc
  src/core/db.cc
  src/core/dto.cc
)

add_library(corelib STATIC ${CORELIB_SOURCES})

target_link_libraries(corelib
  PUBLIC
    protolibsnc
    SQLite::SQLite3
    ${LIBSODIUM_LIBRARIES}
    dfs
)

target_include_directories(corelib
  PUBLIC
    ${LIBSODIUM_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/dfs/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)


# === Main Executable ===
add_executable(storage_node_core src/main.cc)
target_link_libraries(storage_node_core PRIVATE corelib)

# === Test Executables ===
add_executable(test_sqlite3 test/test_sqlite3.cc)
add_executable(test_db_wrapper test/test_db_wrapper.cc)
add_executable(test_db_write test/test_db_write.cc)

target_link_libraries(test_sqlite3 PRIVATE corelib)
target_link_libraries(test_db_wrapper PRIVATE corelib)
target_link_libraries(test_db_write PRIVATE corelib)