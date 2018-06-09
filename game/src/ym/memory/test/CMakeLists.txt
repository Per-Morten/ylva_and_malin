add_executable(ym_allocator_test
    allocator_tests.c
)

target_include_directories(ym_allocator_test
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ym_allocator_test
    ym_core
    ym_memory
    ym_test
)
