add_library(ym_std
    types/ym_types.h
    memory/ym_memory.h
)

target_include_directories(ym_std PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/memory"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory/heap_allocator"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory/stack_frame_allocator"
    "${CMAKE_CURRENT_SOURCE_DIR}/container"
    "${CMAKE_CURRENT_SOURCE_DIR}/container/fixed_vector"
    "${CMAKE_CURRENT_SOURCE_DIR}/types"
    "${CMAKE_CURRENT_SOURCE_DIR}/construction_destruction"
    "${CMAKE_CURRENT_SOURCE_DIR}/algorithm"
)

target_link_libraries(ym_std
    ym_core
)

set_target_properties(ym_std
    PROPERTIES
        LINKER_LANGUAGE CXX
)

if (${YM_ENABLE_TESTS})
    add_executable(ym_std_tests
        tests/std_tests.cpp
        tests/memory/stack_frame_allocator_test.cpp
        tests/memory/heap_allocator_test.cpp
        tests/container/fixed_vector_test.cpp
    )

    target_link_libraries(ym_std_tests
        ym_std
        catch
    )
endif()
