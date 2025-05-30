cmake_minimum_required(VERSION 3.10)
project(demo_client)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)

add_executable(demo_client src/main.cc)
target_link_libraries(demo_client
  PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
    dfs-core
    dfs-crypto
    dfs-net
)
target_include_directories(demo_client
  PRIVATE
    ${LIBSODIUM_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${CMAKE_BINARY_DIR}/dfs-net
)