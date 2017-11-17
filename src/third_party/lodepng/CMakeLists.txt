add_library(lodepng
    lodepng.c
)
target_include_directories(lodepng INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
