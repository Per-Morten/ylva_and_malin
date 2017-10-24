cmake_minimum_required(VERSION 3.3.2)
project(ylva_and_malin)

string(TOLOWER ${CMAKE_BUILD_TYPE} YM_OUTPUT_FOLDER)
set(CMAKE_BINARY_DIR "$ENV{YM_BUILD_DIR}/${YM_OUTPUT_FOLDER}")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Compiler setup
set(CMAKE_C_COMPILER gcc-7)
set(CMAKE_CXX_COMPILER g++-7)

set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS}       \
    -std=c11                \
    -Wall                   \
    -Wextra                 \
    -Wpadded                \
    "
)

set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS}       \
    -DYM_ASSERT_TERMINATE   \
    -g3                     \
    -ggdb                   \
    -O0                     \
    "
)

set(CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE}   \
    -DYM_ASSERT_REPORT          \
    -O3                         \
    "
)

subdirs(
    ym
    third_party
)

if (${YM_ENABLE_TESTS})
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS}       \
        -UYM_ASSERT_TERMINATE   \
        -DYM_ASSERT_ERRC        \
        "
    )

    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS}     \
        -Wall                   \
        -Wextra                 \
        -O3                     \
        -std=c++17              \
        "
        )
endif()

if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Test")
    add_executable(main main.c)

    target_link_libraries(main
        ym_core
        ym_graphics
        ym_memory
        ym_telemetry
    )
endif()
