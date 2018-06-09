# Remove these in future.
add_executable(ym_test_exe
    main.c
)

target_include_directories(ym_test_exe
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ym_test_exe
    ym_core
    ym_memory
)

add_library(ym_test INTERFACE
)

target_include_directories(ym_test INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
