add_library(ym_telemetry
    ym_telemetry.c
)

target_include_directories(ym_telemetry
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ym_telemetry
    ym_core
    ym_memory
    pthread
)
