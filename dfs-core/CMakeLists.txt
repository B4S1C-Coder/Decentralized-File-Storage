cmake_minimum_required(VERSION 3.20)
project(dfs-core)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# libsodium
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)
include_directories(${LIBSODIUM_INCLUDE_DIRS})

set(DFS_SOURCES
  sequential_file_splitter.cc
  util.cc
  thread_pool.cc
  diagnostics.cc
  logger.cc
)

add_library(dfs-core ${DFS_SOURCES})
target_include_directories(dfs-core
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(dfs-core ${LIBSODIUM_LIBRARIES} dfs-crypto)

add_executable(test_reconst_unenc test_file_reconst_unenc.cc)
add_executable(test_endian test_endian.cc)
add_executable(test_log test_log.cc)

target_link_libraries(test_reconst_unenc dfs-core)
target_link_libraries(test_log dfs-core)