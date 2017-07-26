add_library(ym_core
    ym_error.c
    ym_log.c
)

target_include_directories(ym_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

