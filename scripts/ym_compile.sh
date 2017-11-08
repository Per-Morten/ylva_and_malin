#!/bin/bash

ym_internal_compile()
{
    local windows_toolset=""
    if [ "$(uname)" ==  "MINGW64_NT-10.0" ];
    then
        windows_toolset=-T"v140_clang_c2"
    fi

    cmake ${windows_toolset} -DCMAKE_BUILD_TYPE=${1} ${YM_SOURCE_DIR}

    if [ "$(uname)" != "MINGW64_NT-10.0" ];
    then
        make "-j8"
    fi
}

ym_internal_print_compile_options()
{
    printf "Compilation options:"
    printf "\n\t- debug"
    printf "\n\t- release"
    printf "\n\t- test"
    printf "\n\n\n"
}

ym_compile()
{
    pushd ${YM_ROOT_DIR} >/dev/null
    local build_type="release"
    if [ ${#} -gt 0 ];
    then
        # Set to lowercase
        build_type=${1,,}
    else
        ym_internal_print_compile_options
    fi

    printf "Building with type: %s\n" ${build_type}

    if [ ! -d "build" ];
    then
        mkdir build
    fi
    pushd build >/dev/null

    # Setup release
    if [ ! -d ${build_type} ];
    then
        mkdir ${build_type}
    fi

    pushd ${build_type} >/dev/null
    # Uppercase first letter
    ym_internal_compile ${build_type,,^}

    popd >/dev/null

    # Pop out of the rest
    popd >/dev/null
    popd >/dev/null
}

ym_build_tests()
{
    pushd ${YM_ROOT_DIR} >/dev/null
    if [ ! -d "build" ];
    then
        mkdir build
    fi
    pushd build >/dev/null

    # Setup test
    if [ ! -d "test" ];
    then
        mkdir test
    fi

    pushd test >/dev/null
    cmake -DCMAKE_BUILD_TYPE=Test -DYM_ENABLE_TESTS:BOOLEAN=1 ${YM_SOURCE_DIR}
    make "-j8"

    popd >/dev/null

    # Pop out of the rest
    popd >/dev/null
    popd >/dev/null
}

ym_run_tests()
{
    pushd ${YM_ROOT_DIR} >/dev/null
    cd build/test/bin
    for file in ./*
    do
        ./${file} "$@"
    done

    popd >/dev/null
}
