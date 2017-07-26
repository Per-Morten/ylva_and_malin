add_library(ym_graphics
    ym_gfx.c
)

target_include_directories(ym_graphics
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_link_libraries(ym_graphics
    ym_core
    ym_memory
    X11
    GL
    GLU
)
