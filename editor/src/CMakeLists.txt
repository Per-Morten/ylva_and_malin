set(headers
    logger.h
    third_party/imgui.h
)

set(source
    third_party/imgui.cpp
    third_party/imgui_draw.cpp
    third_party/imgui_demo.cpp
    third_party/imgui_impl_sdl_gl3
)

add_executable(main main.cpp ${headers} ${source})
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
