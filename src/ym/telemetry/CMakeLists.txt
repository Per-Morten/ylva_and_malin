set(headers
    ym_telemetry.h
)

set(source
    ym_telemetry.c
)

add_library(ym_telemetry
    ${headers}
    ${source}
)

target_include_directories(ym_telemetry
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ym_telemetry
    ym_core
    ym_memory
)

if (WIN32)

else()
    target_link_libraries(ym_telemetry
        pthread
    )
endif()
