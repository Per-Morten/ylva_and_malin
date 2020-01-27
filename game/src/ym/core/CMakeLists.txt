set(headers
    ym_assert.h
    ym_atomic.h
    ym_attributes.h
    ym_core.h
    ym_error.h
    ym_log.h
    ym_math.h
    ym_time.h
    ym_types.h
)

set(source
    ym_error.c
    ym_log.c
)

add_library(ym_core
    ${headers}
    ${source}
)

target_include_directories(ym_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

if (WIN32)

else()
    target_link_libraries(ym_core
        m #math
    )
endif()
