set(headers
    ym_gfx.h
    ym_gfx_gl.h
    ym_gfx_gl_procs.h
)

set(source
    ym_gfx_gl.c
    ym_gfx_gl_init.c
)

if (WIN32)
    set(headers
        ${headers}
        glext.h
        wglext.h
        ym_gfx_gl_win.h
    )

    set(source
        ${source}
        ym_gfx_win.c
    )
else()
    set(source
        ${source}
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
