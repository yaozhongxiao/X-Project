#!/usr/bin/env bash
# 
# Copyright 2020 Develop Group Participants. All rights reserved.
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

ROOT_DIR=`pwd`
#exec=echo 
exec= 

usage() {
    echo ""
    echo "Usage: $0 command [options]"
    echo "command:"
    echo "    -b[--build]    build the project"
    echo "    -h[--help]:    show help message"
    echo "    -c[--clean]:   clean building output"
    echo "    -p[--pack]:    pack output of the project"
    echo ""
    exit 1
}

pack() {
    echo "packing"
    # zip ${full_package} -r ${package_rootDir}/*
}

build() {
    if [ ! -d build ];then
        $exec mkdir build
    fi
    cd ./build
    # cmake ..
    $exec cmake -DCMAKE_BUILD_TYPE=Debug -DX_TRACE=ON ..
    $exec make -j4
}

cleanup() {
    if [ -d build ];then
        $exec rm -rf build
        if [ -z $exec ];then
            echo "try to remove build"
        fi
        $exec mkdir build
    fi
    if [ -d bin ];then
        $exec rm -rf bin
        if [ -z $exec ];then
            echo "try to remove bin"
        fi
        $exec mkdir bin
    fi
}

command_exec() {
    if [ $# -le 0 ]; then
        usage $@
        return 1
    fi

    case "$1" in
        -p | --pack)             
            shift
            pack $@
            ;;
        -b | --build)
            shift
            build $@
            ;;
        -h | --help)
            # shift
            usage $@
            ;;
        -c | --clean)
            shift
            cleanup $@
            ;;
        *)
            usage;;
    esac
    echo ""
    echo "done ..."
}

# actually execute the commands
command_exec $@
