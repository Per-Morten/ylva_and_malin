#!/bin/bash

ym_generate_doxygen()
{
    pushd ${YM_ROOT_DIR} >/dev/null
    doxygen ./doc/doxygen/ym_doxyfile

    xdg-open ./doc/doxygen/html/index.html > /dev/null
    popd >/dev/null
}
