add_library(lodepng
    lodepng.c
)
target_include_directories(lodepng INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_source_files_properties(lodepng.c PROPERTIES COMPILE_FLAGS -Wno-deprecated-declarations)
