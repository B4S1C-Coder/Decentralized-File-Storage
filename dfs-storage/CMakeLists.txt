cmake_minimum_required(VERSION 3.20)
project(dfs-storage)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(SQLite3 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)
include_directories(${LIBSODIUM_INCLUDE_DIRS})

set(DFS_STORAGE_SOURCES
  config.cc
  core.cc
  db.cc
  dto.cc
  orm.cc
)

add_library(dfs-storage ${DFS_STORAGE_SOURCES})
target_include_directories(dfs-storage
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(dfs-storage ${LIBSODIUM_LIBRARIES} dfs-core)

# Add test executables for ORM
add_executable(test_orm test_orm.cc)
target_link_libraries(test_orm dfs-storage)

add_executable(test_repository test_repository.cc)
target_link_libraries(test_repository dfs-storage)