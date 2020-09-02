#
# Copyright 2020 Develop Group Participants. All right reserver.
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
# Date : 2020-03-20
#

include(ExternalProject)
set(JSONCONS_FOUND FALSE)
set(JC_NAME jsoncons-0.149.0.tar.gz)
set(JC_DEFAULT_REPOSITORY https://codeload.github.com/danielaparker/jsoncons/tar.gz/v0.149.0)
option(JC_REPOSITORY "set repository for source code" ${JC_DEFAULT_REPOSITORY})
if(NOT JC_REPOSITORY)
  set(JC_REPOSITORY ${JC_DEFAULT_REPOSITORY})
endif()
message(STATUS "try to download Jsoncons code from ${JC_REPOSITORY}")


option(BUILD_LOCAL "set the source code to build" OFF)
set(JC_UPDATE_DISCONNECTED 0)
if(BUILD_LOCAL)
  set(JC_UPDATE_DISCONNECTED 1)
endif()

set(JC_SOURCE_PREFIX ${CMAKE_SOURCE_DIR}/third-party/src/jsoncons)
set(JC_BUILD_PREFIX ${CMAKE_BINARY_DIR}/third-party/build)
ExternalProject_Add(jsoncons_
    PREFIX ${JC_BUILD_PREFIX}
    LOG_DOWNLOAD 1
    DOWNLOAD_NO_PROGRESS 0
    DOWNLOAD_NAME ${JC_NAME}
    URL ${JC_REPOSITORY}
    LOG_UPDATE 1
    SOURCE_DIR ${JC_SOURCE_PREFIX}
    UPDATE_DISCONNECTED ${JC_UPDATE_DISCONNECTED}
    LOG_CONFIGURE 1
    LOG_BUILD 1
    BUILD_IN_SOURCE 0
    CMAKE_ARGS ""
    LOG_INSTALL 1
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(jsoncons_ SOURCE_DIR)
set(JSONCONS_INCLUDE_DIR "${SOURCE_DIR}/include/")

add_custom_target(jsoncons)
add_dependencies(jsoncons jsoncons_)
set(JSONCONS_FOUND TRUE)
