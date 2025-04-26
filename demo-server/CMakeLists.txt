cmake_minimum_required(VERSION 3.10)
project(demo_server)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)

add_executable(demo_server src/main.cc)

target_link_libraries(demo_server
  PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
    dfs
)

target_include_directories(demo_server
  PRIVATE
    ${LIBSODIUM_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/dfs/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    ${CMAKE_BINARY_DIR}/dfs
)