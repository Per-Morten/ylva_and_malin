add_executable(ym_container_tests

)

target_include_directories(ym_container_tests
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ym_container_tests
    ym_core
    ym_memory
    ym_test
)
