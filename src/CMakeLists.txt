cmake_minimum_required(VERSION 3.3.2)
project(ylva_and_malin)

string(TOLOWER ${CMAKE_BUILD_TYPE} YM_OUTPUT_FOLDER)
set(CMAKE_BINARY_DIR "$ENV{YM_BUILD_DIR}/${YM_OUTPUT_FOLDER}")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)


# Compiler setup
if (NOT WIN32)
    set(CMAKE_C_COMPILER gcc)
endif()

set(YM_STANDARD_FLAGS "")
set(YM_DEBUG_FLAGS "")
set(YM_RELEASE_FALGS "")


set(YM_STANDARD_FLAGS "${YM_STANDARD_FLAGS}  \
                       -std=c11              \
                       -Wall                 \
                       -Wextra               \
                       "
)

set(YM_DEBUG_FLAGS    "${YM_DEBUG_FLAGS}     \
                       -g3                   \
                       -ggdb                 \
                       -O0                   \
                       -DYM_ASSERT_TERMINATE \
                      "
)

set(YM_RELEASE_FLAGS  "${YM_RELEASE_FLAGS}   \
                       -O3                   \
                       -DYM_ASSERT_REPORT    \
                      "
)

set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS}       \
     ${YM_STANDARD_FLAGS}   \
    "
)

set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS}       \
     ${YM_DEBUG_FLAGS}      \
    "
)

set(CMAKE_C_FLAGS_RELEASE
    "${CMAKE_C_FLAGS_RELEASE}   \
     ${YM_RELEASE_FLAGS}        \
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
endif()

if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Test")
    add_executable(main main.c)

    target_link_libraries(main
        ym_core
        ym_graphics
        ym_memory
        ym_telemetry
        ym_resource
    )

    set_target_properties(main PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$ENV{YM_ROOT_DIR}")
endif()
