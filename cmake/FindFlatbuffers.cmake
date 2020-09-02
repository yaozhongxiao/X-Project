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

include(ExternalProject)
set(FLATBUFFERS_FOUND FALSE)

set(FB_NAME flatbuffers-1.11.0.tar.gz)
set(FB_DEFAULT_REPOSITORY https://github.com/google/flatbuffers/archive/v1.11.0.tar.gz)
option(FB_REPOSITORY "set repository for source code" ${FB_DEFAULT_REPOSITORY})
if(NOT FB_REPOSITORY)
  set(FB_REPOSITORY ${FB_DEFAULT_REPOSITORY})
endif()
message(STATUS "try to download FB code from ${FB_REPOSITORY}")


option(BUILD_LOCAL "set the source code to build" OFF)
set(FB_UPDATE_DISCONNECTED 0)
if(BUILD_LOCAL)
  set(FB_UPDATE_DISCONNECTED 1)
endif()

set(FB_SOURCE_PREFIX ${CMAKE_SOURCE_DIR}/third-party/src/flatbuffers)
set(FB_BUILD_PREFIX ${CMAKE_BINARY_DIR}/third-party/build/)
ExternalProject_Add(flatbuffers_
    PREFIX ${FB_BUILD_PREFIX}
    LOG_DOWNLOAD 1
    DOWNLOAD_NO_PROGRESS 0
    DOWNLOAD_NAME ${FB_NAME}
    URL ${FB_REPOSITORY}
    URL_MD5 02c64880acb89dbd57eebacfd67200d8
    LOG_UPDATE 1
    SOURCE_DIR ${FB_SOURCE_PREFIX}
    UPDATE_DISCONNECTED ${FB_UPDATE_DISCONNECTED}
    LOG_CONFIGURE 1
    LOG_BUILD 1
    BUILD_IN_SOURCE 0
    CMAKE_ARGS "-DFLATBUFFERS_BUILD_FLATC=ON;-DFLATBUFFERS_BUILD_FLATLIB=ON"
    LOG_INSTALL 1
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(flatbuffers_ SOURCE_DIR)
ExternalProject_Get_Property(flatbuffers_ BINARY_DIR)
set(FLATBUFFERS_INCLUDE_DIR "${SOURCE_DIR}/include/")
set(FLATBUFFERS_LIB_DIR "${SOURCE_DIR}/")
set(FLATBUFFERS_LIBRARIES "${BINARY_DIR}/libflatbuffers.a")

add_library(flatbuffers STATIC IMPORTED)
set_property(TARGET flatbuffers PROPERTY IMPORTED_LOCATION ${FLATBUFFERS_LIBRARIES})
add_dependencies(flatbuffers flatbuffers_)

#-----------------------------------------------------------------#
#             funtion to generate c header from .fbs              #
#-----------------------------------------------------------------#
set(FLATCC ${BINARY_DIR}/flatc)
function(FBS_TARGET Target)
  cmake_parse_arguments(ARGS "" "OUTPUT_PATH" "INCLUDES;OPTIONS;INPUTS" ${ARGN})
  if(NOT Target)
    message(FATAL_ERROR "FBS_TARGET expect a target name")
  endif()

  if(NOT ARGS_INPUTS)
    message(FATAL_ERROR "FBS_TARGET expect input files")
  endif()

  message("COMMAND ${FLATCC} ${ARGS_OPTIONS} -o ${ARGS_OUTPUT_PATH} ${ARGS_INPUTS}")
  add_custom_target(${Target} ALL
      COMMAND ${FLATCC} ${ARGS_OPTIONS} -o ${ARGS_OUTPUT_PATH} ${ARGS_INPUTS}
      SOURCES ${ARGS_INPUTS}
      DEPENDS ${ARGS_INPUTS}
      COMMENT "[FBS][${Target}] gen fbs to c++ .h"
  )
  add_dependencies(${Target} flatbuffers)
endfunction()

set(FLATBUFFERS_FOUND TRUE)
