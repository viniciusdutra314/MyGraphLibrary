cmake_minimum_required(VERSION 3.10)

project(ShortestPaths C CXX) 

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

option(COMPARE_WITH_IGRAPH "Compare with igraph library" OFF)
option(COMPARE_WITH_C11_THREADS "Compare OpenMP with C11 Threads" OFF)
include_directories(src/include)

add_compile_options(-Wall -Wextra -pedantic -Wpedantic -Werror)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -fsanitize=undefined -fno-omit-frame-pointer)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native)
endif()

add_executable(main_cli src/main.c src/graph_library.c src/data_structures.c)

target_link_libraries(main_cli PRIVATE OpenMP::OpenMP_C)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(main_cli PRIVATE -fsanitize=address -fsanitize=undefined)
endif()

if(COMPARE_WITH_IGRAPH)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(IGRAPH REQUIRED igraph)
    target_link_libraries(main_cli PRIVATE ${IGRAPH_LIBRARIES})
    target_include_directories(main_cli PRIVATE ${IGRAPH_INCLUDE_DIRS})
    target_compile_definitions(main_cli PRIVATE COMPARE_WITH_IGRAPH)
endif()
if(COMPARE_WITH_C11_THREADS)
    target_compile_definitions(main_cli PRIVATE COMPARE_WITH_C11_THREADS)
endif()