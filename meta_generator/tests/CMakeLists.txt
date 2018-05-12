add_executable(tests tests.cpp)

target_link_libraries(tests
meta
)

target_include_directories(tests PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
