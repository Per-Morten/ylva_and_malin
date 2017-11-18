set(headers
    ym_memory.h
)

set(source
    ym_memory.c
)

add_library(ym_memory
    ${headers}
    ${source}
)

target_include_directories(ym_memory
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ym_memory
    ym_core
)
