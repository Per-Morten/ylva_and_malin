#!/bin/bash

# Export environment variables
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

pushd ${script_dir}/../ >/dev/null
export YM_ROOT_DIR=$(pwd)
popd >/dev/null

export YM_BUILD_DIR=${YM_ROOT_DIR}/build
export YM_SOURCE_DIR=${YM_ROOT_DIR}/game/src
export YM_SETUP_DIR=${YM_ROOT_DIR}/setup
export YM_SCRIPTS_DIR=${YM_ROOT_DIR}/scripts
export YM_RESOURCE_DIR=${YM_ROOT_DIR}/resources

source ${YM_SCRIPTS_DIR}/ym_compile.sh
source ${YM_SCRIPTS_DIR}/ym_valgrind.sh
source ${YM_SCRIPTS_DIR}/ym_doxygen.sh
source ${YM_SCRIPTS_DIR}/ym_line_count.sh
