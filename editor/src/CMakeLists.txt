cmake_minimum_required(VERSION 3.3.2)
project(editor)

set(CMAKE_BINARY_DIR "$ENV{YM_BUILD_DIR}/editor")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK REQUIRED gtk+-2.0)
    set(CMAKE_CXX_COMPILER clang++-5.0)
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS}      \
        -std=c++17               \
        -Wall                    \
        -DNOC_FILE_DIALOG_GTK    \
        -O3                      \
        "
    )
        #-Wextra                  \

else()
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS}      \
        /std:c++17               \
        /W3                      \
        /DNOC_FILE_DIALOG_WIN32  \
        "
    )

endif()


set(headers
    logger.h

    third_party/imgui.h
    third_party/imgui-SFML.h
)

set(source

    third_party/imgui.cpp
    third_party/imgui_draw.cpp
    third_party/imgui_demo.cpp
    third_party/imgui-SFML.cpp
    third_party/lodepng.cpp
)

add_executable(main main.cpp ${headers} ${source})

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Update these for when moving to linux
    target_include_directories(main PUBLIC
                               "${CMAKE_CURRENT_SOURCE_DIR}"
                               "${CMAKE_CURRENT_SOURCE_DIR}/third_party/"
                               "${GTK_INCLUDE_DIRS}"
                               )

    target_link_libraries(main
                          # For weird reason SFML textures don't work when using release libs. Don't bother fixing it, as debug is enough for editor
                          "sfml-audio"
                          "sfml-graphics"
                          #"sfml-main"
                          "sfml-network"
                          "sfml-system"
                          "sfml-window"
                          "${GTK_LIBRARIES}"

                          "GL"
                          stdc++fs
                          )

else()
    # Update these for when moving to linux
    target_include_directories(main PUBLIC
                               "${CMAKE_CURRENT_SOURCE_DIR}"
                               "${CMAKE_CURRENT_SOURCE_DIR}/third_party/"
                               "$ENV{SFML_HOME}/include"
                               )

    target_link_libraries(main
                          # For weird reason SFML textures don't work when using release libs. Don't bother fixing it, as debug is enough for editor
                          "$ENV{SFML_HOME}/lib/sfml-audio-d.lib"
                          "$ENV{SFML_HOME}/lib/sfml-graphics-d.lib"
                          "$ENV{SFML_HOME}/lib/sfml-main-d.lib"
                          "$ENV{SFML_HOME}/lib/sfml-network-d.lib"
                          "$ENV{SFML_HOME}/lib/sfml-system-d.lib"
                          "$ENV{SFML_HOME}/lib/sfml-window-d.lib"

                          "OpenGL32.lib"
                          )

    set_target_properties(main PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$ENV{YM_ROOT_DIR}")

endif()



