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
#

if(BUILD_TESTS)
    file(GLOB_RECURSE X_UNITTESTS_SRCS ./src "*.cpp" "*.cc")
    # find and configure -lthread with CMAKE_THREAD_LIBS_INIT
    find_package(Threads)
    build_executable(x-test
        SOURCES ${X_UNITTESTS_SRCS}
        LIBS libgtest base dl pthread
        INSTALL DESTINATION test
    )
    if (COMPILER_IS_GNU)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
    endif ()
endif()
