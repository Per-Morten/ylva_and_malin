cmake_minimum_required(VERSION 3.3.2)
project(ylva_and_malin CXX)

string(TOLOWER ${CMAKE_BUILD_TYPE} YM_OUTPUT_FOLDER)
set(CMAKE_BINARY_DIR "$ENV{YM_BUILD_DIR}/${YM_OUTPUT_FOLDER}")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Compiler setup
set(CMAKE_CXX_COMPILER g++-7)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS}     \
    -std=c++1z              \
    -Wall                   \
    -Wextra                 \
    -Wreorder               \
    -DYM_ASSERT_TERMINATE   \
    "
)

set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS}     \
    -g3                     \
    -ggdb                   \
    -O0                     \
    "
)

set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} \
    -O3                         \
    "
)

subdirs(
    ym
    third_party
)

if (${YM_ENABLE_TESTS})
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS}     \
        -UYM_ASSERT_TERMINATE   \
        -DYM_ASSERT_THROW       \
        "
    )
endif()

if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Test")
    add_executable(main main.cpp)

    target_link_libraries(main
        ym_std
        ym_macros
    )
endif()
