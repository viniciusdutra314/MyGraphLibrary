cmake_minimum_required(VERSION 3.10)

project(ShortestPaths C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(src/include)

add_compile_options(-Wall -Wextra -pedantic -Wpedantic -Werror)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -fsanitize=address,undefined -fno-omit-frame-pointer)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native)
endif()

add_executable(main_cli src/main.c src/graph_library.c src/data_structures.c)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(main_cli PRIVATE -fsanitize=address -fsanitize=undefined)
endif()
