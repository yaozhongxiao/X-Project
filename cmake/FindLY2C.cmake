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

#---------------------------------------------------------#
#          funtion to generate lexer from .l              #
#---------------------------------------------------------#
set(FLEX_EXE flex)
function(FLEX_TARGET Target)
  cmake_parse_arguments(PARSED_ARGS "" "NAME;INPUT;OUTPUT" "OPTIONS;DEPENDS" ${ARGN})

  if(NOT Target)
    message(FATAL_ERROR "FLEX_TARGET expect a target name")
  endif(NOT Target)

  if(NOT PARSED_ARGS_OUTPUT)
    message(FATAL_ERROR "FLEX_TARGET expect an output filename")
  endif(NOT PARSED_ARGS_OUTPUT)

  if(NOT PARSED_ARGS_INPUT)
    message(FATAL_ERROR "FLEX_TARGET expect an input filename")
  endif(NOT PARSED_ARGS_INPUT)

  message("COMMAND ${FLEX_EXE} ${PARSED_ARGS_OPTIONS} -o ${PARSED_ARGS_OUTPUT} ${PARSED_ARGS_INPUT}")
  add_custom_command(
      OUTPUT ${PARSED_ARGS_OUTPUT}
      COMMAND ${FLEX_EXE} ${PARSED_ARGS_OPTIONS} -o ${PARSED_ARGS_OUTPUT} ${PARSED_ARGS_INPUT}
      DEPENDS ${PARSED_ARGS_INPUT} ${PARSED_ARGS_DEPENDS}
      COMMENT "[FLEX][${Target}] Building lexer with flex"
  )
  add_custom_target(
      ${Target}
      SOURCES ${PARSED_ARGS_INPUT}
      DEPENDS ${PARSED_ARGS_OUTPUT}
  )
endfunction()


#---------------------------------------------------------#
#          funtion to generate parser from .y             #
#---------------------------------------------------------#
set(BISON_EXE bison)
function(BISON_TARGET Target)
  cmake_parse_arguments(PARSED_ARGS "" "NAME;INPUT;OUTPUT" "OPTIONS;DEPENDS" ${ARGN})

  if(NOT Target)
      message(FATAL_ERROR "BISON_TARGET expect a target name")
  endif(NOT Target)

  if(NOT PARSED_ARGS_OUTPUT)
      message(FATAL_ERROR "BISON_TARGET expect an output filename")
  endif(NOT PARSED_ARGS_OUTPUT)
  if(NOT PARSED_ARGS_INPUT)
      message(FATAL_ERROR "BISON_TARGET expect an input filename")
  endif(NOT PARSED_ARGS_INPUT)

  add_custom_command(
      OUTPUT ${PARSED_ARGS_OUTPUT}
      COMMAND ${BISON_EXE} ${PARSED_ARGS_OPTIONS} -o ${PARSED_ARGS_OUTPUT} ${PARSED_ARGS_INPUT}
      DEPENDS ${PARSED_ARGS_INPUT} ${PARSED_ARGS_DEPENDS}
      COMMENT "[BISON][${Target}] Building parser with bison"
  )
  add_custom_target(
      ${Target}
      SOURCES ${PARSED_ARGS_INPUT}
      DEPENDS ${PARSED_ARGS_OUTPUT}
  )
endfunction()

