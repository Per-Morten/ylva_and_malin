cmake_minimum_required(VERSION 3.3.2)
project(editor)

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	message("If was true")
	set(CMAKE_CXX_COMPILER clang++-5.0)
	set(CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS}      \
		-std=c++17               \
		-Wall                    \
		-Wextra                  \
		"
	)
else()
	set(CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS}      \
		/std:c++17               \
		/W3						 \
		"
	)
endif()


subdirs(
    src
#    tests
)

