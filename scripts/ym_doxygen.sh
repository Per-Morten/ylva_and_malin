#!/bin/bash

ym_generate_doxygen()
{
    pushd ${YM_ROOT_DIR} >/dev/null
    doxygen ./docs/doxygen/ym_doxyfile

    xdg-open ./docs/doxygen/html/index.html > /dev/null
    popd >/dev/null
}
