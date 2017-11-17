set(headers
    ym_thread.h
)

if (WIN32)
    set(source
        ym_thread_win.c
    )
else()
    set(source
        ym_thread_unix.c
    )
endif()

add_library(ym_thread
    ${headers}
    ${source}
)

target_include_directories(ym_thread
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ym_thread
    ym_core
)

if (WIN32)

else()
    target_link_libraries(ym_graphics
        pthread
    )
endif()
