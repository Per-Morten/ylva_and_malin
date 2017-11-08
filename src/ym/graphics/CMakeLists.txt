set(headers
    ym_gfx.h
)

set(source
    ym_gfx.c
)

if (WIN32)
set(source
    ym_gfx_win.c
)
else()
set(source
    ym_gfx_unix.c
)
endif()

add_library(ym_graphics
    ${headers}
    ${source}
)

target_include_directories(ym_graphics
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ym_graphics
    ym_core
    ym_memory
)

if (WIN32)
    target_link_libraries(ym_graphics
        OpenGL32
    )
else()
    target_link_libraries(ym_graphics
        X11
        GL
        GLU
    )
endif()
