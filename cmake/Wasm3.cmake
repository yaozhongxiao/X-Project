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

set(Wasm3_FOUND false)

option(BUILD_LOCAL "set the source code to build" ON)

set(XPROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)
set(THIRDPARTY_ROOT ${XPROJECT_ROOT}/third-party)

# ATTENTION(zhongxiao.yzx): wasm3 adopt the "DEBUG" to control
# the class members' existence during implementation, if the
# "DEBUG" definition is miss-matched, the class layout will
# miss-match too, which will lead to disaster!!!
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG=1")

# 1. set google/benchmark source code branch
set(WASM3_DEFAULT_BRANCH main)
option(WASM3_BRANCH "select branch for wasm3" ${WASM3_DEFAULT_BRANCH})
# option only be initialized to ON or OFF, otherwise could be set by cmake -DWASM3_BRANCH=X
if(NOT WASM3_BRANCH)
    set(WASM3_BRANCH ${WASM3_DEFAULT_BRANCH})
endif()

# 2. set source code repository
set(WASM3_DEFAULT_REPOSITORY git@github.com:wasm3/wasm3.git)
option(WASM3_REPOSITORY "set repository for source code" ${WASM3_DEFAULT_REPOSITORY})
if(NOT WASM3_REPOSITORY)
    set(WASM3_REPOSITORY ${WASM3_DEFAULT_REPOSITORY})
endif()

message(STATUS "try to build ${CMAKE_BUILD_TYPE} version ${WASM3_REPOSITORY}:${WASM3_BRANCH}")

set(WASM3_DOWNLOAD_DIR ${THIRDPARTY_ROOT}/wasm3)
if(NOT EXISTS ${WASM3_DOWNLOAD_DIR})
    add_custom_command(OUTPUT ${WASM3_DOWNLOAD_DIR}
        # download the file if we need it.
        COMMAND echo "download wasm3 from ${WASM3_REPOSITORY}:${WASM3_BRANCH} -> ${WASM3_DOWNLOAD_DIR}"
        COMMAND sh ${WASM_ANDROID_ROOT}/cmake/shell/wasm3_download.sh -b ${WASM3_BRANCH} -o ${WASM3_DOWNLOAD_DIR} ${WASM3_REPOSITORY}
        # COMMAND git clone ${WASM3_REPOSITORY} -b ${WASM3_BRANCH} ${WASM3_DOWNLOAD_DIR}
        VERBATIM)
endif()
add_custom_target(wasm_engine_download DEPENDS ${WASM3_DOWNLOAD_DIR})

#########################################################################
# buildconfig from ${M3_HOME}/platforms/android/app/src/main/cpp
set(WASM3_SRC_DIR ${WASM3_DOWNLOAD_DIR}/source)
set(WASM3_HEADER_DIR ${WASM3_DOWNLOAD_DIR})

add_definitions(-DANDROID -Wno-format-security -O3
                #-fno-optimize-sibling-calls
                -flto -fomit-frame-pointer -fno-stack-check -fno-stack-protector)

# FIXME(zhongxiao.yzx):
# file(GLOB ...) can not handle the generated files correctly,
# try to find a better way to list sources automatically
# file(GLOB M3_SRC FOLLOW_SYMLINKS "${WASM3_SRC_DIR}/*.c" "*.c")
set(M3_SRC
    "${WASM3_SRC_DIR}/extensions/m3_extensions.c"
    "${WASM3_SRC_DIR}/m3_api_libc.c"
    "${WASM3_SRC_DIR}/m3_api_meta_wasi.c"
    "${WASM3_SRC_DIR}/m3_api_tracer.c"
    "${WASM3_SRC_DIR}/m3_api_uvwasi.c"
    "${WASM3_SRC_DIR}/m3_api_wasi.c"
    "${WASM3_SRC_DIR}/m3_bind.c"
    "${WASM3_SRC_DIR}/m3_code.c"
    "${WASM3_SRC_DIR}/m3_compile.c"
    "${WASM3_SRC_DIR}/m3_core.c"
    "${WASM3_SRC_DIR}/m3_env.c"
    "${WASM3_SRC_DIR}/m3_exec.c"
    "${WASM3_SRC_DIR}/m3_function.c"
    "${WASM3_SRC_DIR}/m3_info.c"
    "${WASM3_SRC_DIR}/m3_module.c"
    "${WASM3_SRC_DIR}/m3_parse.c"
)
message("WASM3_SRC_DIR ${M3_SRC}")
set_source_files_properties(${M3_SRC}
                            PROPERTIES GENERATED TRUE)

add_library(wasm-engine-lib STATIC ${M3_SRC})
add_dependencies(wasm-engine-lib  wasm_engine_download)

set_target_properties(wasm-engine-lib PROPERTIES LINK_FLAGS_RELEASE "-flto -O3")
target_include_directories(wasm-engine-lib PUBLIC ${WASM3_HEADER_DIR} ${WASM3_SRC_DIR})
set_target_properties(wasm-engine-lib PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(wasm-engine-lib PROPERTIES OUTPUT_NAME wasm3)
set(wasm_engline-include ${WASM3_HEADER_DIR})

# # remove the downloaded files after building
# # otherwise, "./gradlew clean" will be failed
# add_custom_command(TARGET wasm-engine-lib POST_BUILD
#     COMMAND rm -rf  ${WASM3_DOWNLOAD_DIR}
#     COMMAND ${CMAKE_COMMAND} -E echo "rm -rf  ${WASM3_DOWNLOAD_DIR}"
#     VERBATIM
# )

set(Wasm3_FOUND true)
