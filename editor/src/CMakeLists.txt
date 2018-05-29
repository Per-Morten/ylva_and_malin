cmake_minimum_required(VERSION 3.3.2)
project(editor)

set(CMAKE_BINARY_DIR "$ENV{YM_BUILD_DIR}/editor")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_COMPILER clang++-5.0)
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS}      \
        -std=c++17               \
        -Wall                    \
        -Wextra                  \
        "
    )
else()
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS}      \
        /std:c++17               \
        /W3                      \
        "
    )
endif()


set(headers
    logger.h
    texture.h

    third_party/imgui.h
)

set(source
    texture.cpp

    third_party/imgui.cpp
    third_party/imgui_draw.cpp
    third_party/imgui_demo.cpp
    third_party/imgui_impl_sdl_gl3.cpp
    third_party/lodepng.cpp
)

add_executable(main main.cpp ${headers} ${source})

# Update these for when moving to linux
target_include_directories(main PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/"
    "$ENV{SDL_HOME}/include"
    "$ENV{GLEW_HOME}/include"
)

target_link_libraries(main
    "$ENV{SDL_HOME}/lib/SDL2.lib"
    "$ENV{GLEW_HOME}/lib/glew32.lib"
    "OpenGL32.lib"
)

set_target_properties(main PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$ENV{YM_ROOT_DIR}")
