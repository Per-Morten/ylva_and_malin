set(headers
    meta.h
    logger.h
)

set(source
    meta.cpp
)

add_library(meta
    ${headers}
    ${source}
)

target_include_directories(meta PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_executable(main main.cpp)
