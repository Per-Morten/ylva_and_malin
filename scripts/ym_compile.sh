#!/bin/bash

# ym_internal_compile()
# {
#     local windows_toolset=""
#     if [[ "$(uname)" ==  "MINGW64_NT-10.0" ]];
#     then
#         windows_toolset=-T"v140_clang_c2"
#     fi

#     cmake ${windows_toolset} -DCMAKE_BUILD_TYPE=${1} ${YM_SOURCE_DIR}

#     if [[ "$(uname)" != "MINGW64_NT-10.0" ]];
#     then
#         make "-j8";
#     fi
# }

# ym_internal_print_compile_options()
# {
#     printf "Compilation options:"
#     printf "\n\t- debug"
#     printf "\n\t- release"
#     printf "\n\t- test"
#     printf "\n\t- rebuild | clean"
#     printf "\n\n\n"
# }

# ym_compile()
# {
#     pushd ${YM_ROOT_DIR} >/dev/null
#     local build_type="release"
#     local run=false

#     # Ugly, fix later
#     if [[ ${#} -gt 0 ]];
#     then
#         if [[ ${1} =~ -r ]];
#         then
#             run=true

#             if [[ ${#} -gt 1 ]];
#             then
#                 build_type=${2,,}
#             fi
#         else
#             build_type=${1,,}
#         fi

#         # Set to lowercase
#     else
#         ym_internal_print_compile_options
#     fi

#     if [[ ${build_type} =~ (rebuild|clean) ]];
#     then
#         rm -rf ./build
#         build_type="release"
#     fi

#     printf "Building with type: %s\n" ${build_type}

#     if [[ ! -d "build" ]];
#     then
#         mkdir build
#     fi
#     pushd build >/dev/null

#     # Setup build type dir
#     if [[ ! -d ${build_type} ]];
#     then
#         mkdir ${build_type}
#     fi

#     pushd ${build_type} >/dev/null

#     # Uppercase first letter
#     # Also should check if compilation succeeded
#     # If it didn't we shouldn't run the program
#     ym_internal_compile ${build_type,,^}
#     local compile_res=${?}

#     popd >/dev/null # eo build_type dir

#     # Pop out of the rest
#     popd >/dev/null # eo build dir

#     if [[ ${run} == true && ${compile_res} -eq 0 ]];
#     then
#         ./build/${build_type}/bin/main
#     fi
#     popd >/dev/null # eo YM_ROOT_DIR

#     return ${compile_res}
# }

# $0: Build target
# $1: Build type
ym_internal_compile()
{
    if [[ ${1} == "editor" ]];
    then
        cmake -DCMAKE_BUILD_TYPE=${2} ${YM_ROOT_DIR}/editor/src
    fi

    if [[ ${1} == "game" ]];
    then
        local toolset=""
        if [[ "$(uname)" == "MINGW64_NT-10.0" ]];
        then
            local toolset=-T"v141_clang_c2"
        fi
        cmake ${toolset} -DCMAKE_BUILD_TYPE=${2} ${YM_SOURCE_DIR}
    fi

    if [[ "$(uname)" != "MINGW64_NT-10.0" ]];
    then
        make "-j8";
    fi
}

ym_compile()
{
    pushd ${YM_ROOT_DIR} >/dev/null
    local build_type="release"
    local build_target="game"

    for i in `seq 1 ${#}`;
    do
        if [[ ${!i} =~ -o ]];
        then
            local next=$((i+1));
            build_target=${!next,,};
        fi

        if [[ ${!i} =~ -t ]];
        then
            local next=$((i+1));
            build_type=${!next,,};
        fi

        if [[ ${!i} =~ -h ]];
        then
            printf "Usage: "
            printf "\n\t-o target (editor, game)"
            printf "\n\t-t type (debug, release, test, rebuild | clean"
            printf "\n\n"
        fi
    done

    if [[ ${build_type} =~ clean ]];
    then
        rm -rf ./build
        return
    fi

    printf "Building target: %s - %s\n" ${build_target} ${build_type}

    mkdir -p build/${build_target}/${build_type}
    pushd build/${build_target}/ >/dev/null

    ym_internal_compile ${build_target} ${build_type,,^}
    local compile_res=${?}

    popd >/dev/null
    popd >/dev/null

    return ${compile_res}
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
    local compile_res=${?}

    popd >/dev/null

    # Pop out of the rest
    popd >/dev/null
    popd >/dev/null

    return ${compile_res}
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

ym_continous_game()
{
    # Main idea taken from: https://stackoverflow.com/a/9461685
    chsum1=""
    while [[ true ]]
    do
        chsum2=$(find ${YM_SOURCE_DIR} ${YM_RESOURCE_DIR} -type f -exec md5sum {} \;)
        if [[ $chsum1 != $chsum2 ]] ; then
        clear

            # Get window currently in focus
            work_focus=$(xprop -root | awk '/_NET_ACTIVE_WINDOW/ {print $5; exit}')

            # Launch compilation
            ym_compile

            if [[ $? -eq 0 ]];
            then
                # Kill potentially running ym application
                ym_window_id=$(wmctrl -l | grep -E "ylva_and_malin$" | awk '{ print $1}')
                identified_count=$(echo ${ym_window_id} | wc -l)

                if [[ identified_count -gt 1 ]];
                then
                    printf "Warning, found more than 1 instance of ym: %s\n" ${ym_window_id}
                    wmctrl -l
                else
                    wmctrl -i -c ${ym_window_id} >> /dev/null 2>&1
                fi

                # Run build and set focus back to sublime
                ./build/release/bin/main &

                # Get pid, used to check if it exists in ps | grep
                local ym_pid=$!
                ym_pid=$(ps | grep "${ym_pid}")

                sleep 0.25

                # Find new ym window and move it.
                local new_window_id=""
                while [[ ((-z ${new_window_id+x} || ${new_window_id} == "")) && $(ps | grep "${ym_pid}") != "" ]]
                do
                    new_window_id=$(wmctrl -l 2>/dev/null | grep -E "ylva_and_malin$" | awk '{ print $1}')
                done
                wmctrl -i -r ${new_window_id} -e 0,0,0,-1,-1
                wmctrl -i -a ${work_focus}
            fi

            chsum1=$chsum2
        fi
        sleep 2
    done

}

ym_continous_tests()
{
    # Main idea taken from: https://stackoverflow.com/a/9461685
    chsum1=""
    while [[ true ]]
    do
        chsum2=$(find ${YM_SOURCE_DIR} ${YM_RESOURCE_DIR} -type f -exec md5sum {} \;)
        if [[ $chsum1 != $chsum2 ]] ; then
        clear
            # Launch compilation
            ym_build_tests

            if [[ $? -eq 0 ]];
            then
                #clear
                ym_run_tests
            fi

            chsum1=$chsum2
        fi
        sleep 2
    done

}
