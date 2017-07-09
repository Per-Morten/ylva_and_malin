#!/bin/bash

ym_compile_debug()
{
    pushd ${YM_ROOT_DIR} >/dev/null
    if [ ! -d "build" ];
    then
        mkdir build
    fi
    pushd build >/dev/null

    # Setup debug
    if [ ! -d "debug" ];
    then
        mkdir debug
    fi

    pushd debug >/dev/null
    cmake -DCMAKE_BUILD_TYPE=Debug ${YM_SOURCE_DIR}
    make "-j8"
    popd >/dev/null

    # Pop out of the rest
    popd >/dev/null
    popd >/dev/null
}

ym_compile()
{
    pushd ${YM_ROOT_DIR} >/dev/null
    if [ ! -d "build" ];
    then
        mkdir build
    fi
    pushd build >/dev/null

    # Setup release
    if [ ! -d "release" ];
    then
        mkdir release
    fi

    pushd release >/dev/null
    cmake -DCMAKE_BUILD_TYPE=Release ${YM_SOURCE_DIR}
    make "-j8"
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
