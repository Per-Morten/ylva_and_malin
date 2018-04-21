set(headers
    ym_array.h
)

set(source
)

add_library(ym_container
    ${headers}
    ${source}
)

target_include_directories(ym_container
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ym_container
    ym_core
)

if (${YM_ENABLE_TESTS})
    subdirs(test)
endif()
