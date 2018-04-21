set(headers
    ym_resource.h
)

set(source
    ym_resource.c
)

add_library(ym_resource
    ${headers}
    ${source}
)

target_include_directories(ym_resource
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ym_resource
    ym_core
    ym_memory
    ym_graphics
)
