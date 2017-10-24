add_executable(ym_test
    main.c
)

target_include_directories(ym_test
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

#target_link_libraries(ym_test
#    ym_core
#    ym_memory
#    pthread
#)
