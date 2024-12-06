cmake_minimum_required(VERSION 3.10)
project(scaencryptionlib)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# Find libsodium
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)

include_directories(${LIBSODIUM_INCLUDE_DIRS})

# stream_encrypt_decrypt
add_library(streamencdec src/stream_encrypt_decrypt.cc)
target_include_directories(streamencdec PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(streamencdec ${LIBSODIUM_LIBRARIES})

# executable
add_executable(test src/test.cc)
target_link_libraries(test streamencdec)