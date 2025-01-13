cmake_minimum_required(VERSION 3.10)
project(dfsnclientcorelib)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Find libsodium
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)

include_directories(${LIBSODIUM_INCLUDE_DIRS})

# The actual client-core library

set(CLIENTCORE_SOURCES
  src/stream_encrypt_decrypt.cc
  src/sequential_file_splitter.cc
  src/util.cc
  src/thread_pool.cc
)

add_library(clientcore ${CLIENTCORE_SOURCES})
target_include_directories(clientcore PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(clientcore ${LIBSODIUM_LIBRARIES})

# executable
add_executable(test test/test.cc)
target_link_libraries(test clientcore)