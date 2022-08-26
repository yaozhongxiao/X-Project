#!/usr/bin/env bash
#
# Copyright 2022 Develop Group Participants. All right reserver.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd -P)"
XPROJECT_ROOT=${SCRIPT_DIR}/../..

WASM3_URL=git@github.com:wasm3/wasm3.git
WASM3_BRANCH=main
WASM3_OUTPUT=${XPROJECT_ROOT}/third-party/wasm3

function usage() {
    cat << EOF
    Usage: $0 [options]
    Options:
    --url             Project Git URL
    -b|--branch       Project Git Branch
    -t|--target       Project Build Target
    -o|--output       Project Output Directory
    -d|--debug        Build With Debug Mode (default)
    -r|--release      Build With Release Mode
    -c|--clean)       Clean Build Result
    -h|--help         This Message.
EOF
}

function CleanAll() {
    echo "clean all"
}

function options_parse() {
    while test $# -gt 0; do
        case "$1" in
            --url)
                shift
                WASM3_URL=$1
                ;;
            -b|--branch)
                shift
                WASM3_BRANCH=$1
                ;;
            -o|--output)
                shift
                WASM3_OUTPUT=$1
                ;;
            -t|--target)
                shift
                WASM3_TARGET=$1
                ;;
            -c|--clean)
                CleanAll
                ;;
            -d|--debug)
                BuildType=Debug
                ;;
            -r|--release)
                BuildType=Release
                ;;
            -h|--help)
                usage
                exit 0
                ;;
            *)
                WASM3_URL=$1
                ;;
        esac
        shift
    done
}

# parse options
echo $0 $@
options_parse $@

WASM3_HEADER_OUTPUT=${WASM3_OUTPUT}/wasm3

# download wasm3
if [ -d ${WASM3_OUTPUT} ];then
    echo "rm -rf ${WASM3_OUTPUT}"
    rm -rf ${WASM3_OUTPUT}
fi
echo "git clone ${WASM3_URL} -b ${WASM3_BRANCH} ${WASM3_OUTPUT}"
git clone ${WASM3_URL} -b ${WASM3_BRANCH} ${WASM3_OUTPUT}

if [ -d ${WASM3_HEADER_OUTPUT} -o -L ${WASM3_HEADER_OUTPUT} ];then
    rm -rf ${WASM3_HEADER_OUTPUT}
fi
ln -s ${WASM3_OUTPUT}/source ${WASM3_HEADER_OUTPUT}
