cmake_minimum_required(VERSION 3.3.2)
project(meta_generator)

set(CMAKE_CXX_COMPILER clang++-5.0)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS}       \
     -std=c++17               \
     -Wall                    \
     -Wextra                  \
    "
)

subdirs(
    src
    tests
)

