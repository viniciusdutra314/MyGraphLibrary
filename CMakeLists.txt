cmake_minimum_required(VERSION 3.10)

project(ShortestPaths C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(src)

add_compile_options(${C_COMPILE_FLAGS} -Wall -Wextra -pedantic -Wpedantic -Werror)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native)
endif()

add_executable(floyd_wharsall_cli src/floyd_warshall_cli.c src/graph_library.c)

