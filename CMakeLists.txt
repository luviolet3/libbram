cmake_minimum_required(VERSION 4.2)
project(libbram)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
add_subdirectory(lib)
add_subdirectory(tests)

include(cmake/CPM.cmake)