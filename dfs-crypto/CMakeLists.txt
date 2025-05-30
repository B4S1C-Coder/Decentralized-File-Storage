cmake_minimum_required(VERSION 3.10)
project(dfs-crypto)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# libsodium
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)
include_directories(${LIBSODIUM_INCLUDE_DIRS})

set(DFS_CORE_SOURCES
  stream_encrypt_decrypt.cc
)

add_library(dfs-crypto ${DFS_CORE_SOURCES})
target_include_directories(dfs-crypto
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(dfs-crypto ${LIBSODIUM_LIBRARIES} dfs-core)

add_executable(test_new_enc_dec test_new_enc_dec.cc)
add_executable(test_split_reconst test_split_reconst.cc)
add_executable(test test.cc)

target_link_libraries(test_new_enc_dec dfs-crypto)
target_link_libraries(test_split_reconst dfs-crypto)
target_link_libraries(test dfs-crypto)