add_library(ym_memory
    ym_memory.c
)

target_include_directories(ym_memory
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(ym_memory
    ym_core
)

if (${YM_ENABLE_TESTS})
    add_executable(ym_memory_tests
        tests/memory_tests.cpp
    )

    target_link_libraries(ym_memory_tests
        catch
        ym_memory
        ym_core
    )
endif()
