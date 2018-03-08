#!/bin/bash

ym_line_count()
{
    pushd ${YM_SOURCE_DIR} >/dev/null

    files=$(find | grep -Ev "(third_party|glext.h|wglext.h)" | grep -E "*.[hc]$")
    local line_sum
    for file in ${files}
    do
        count=$(cat ${file} | grep -Ev "([{}]|//.|^\s*$)" | wc -l)
        ((line_sum+=${count}))
    done
    echo ${line_sum}

    popd >/dev/null # EO YM_SOURCE_DIR
}
