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
set(OPENSSL_FOUND true)

include(ExternalProject)

set(OPENSSL_CONFIGURE ./config)
set(OPENSSL_CONFIGURE_ARG "")
ExternalProject_Add(project_openssl
    PREFIX ${CMAKE_SOURCE_DIR}/libraries/third_party/openssl
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/libraries/third_party/openssl
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${OPENSSL_CONFIGURE} ${OPENSSL_CONFIGURE_ARG}
    LOG_CONFIGURE 1
    BUILD_COMMAND CC=${CMAKE_C_COMPILER} $(MAKE)
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(project_openssl SOURCE_DIR)

set(OPENSSL_INCLUDE_DIR "${SOURCE_DIR}/include")
set(OPENSSL_LIB_DIR "${SOURCE_DIR}/")

add_library(openssl-crypto-static STATIC IMPORTED)
#target_link_libraries(openssl-crypto-static INTERFACE dl)
set_property(TARGET openssl-crypto-static PROPERTY IMPORTED_LOCATION "${SOURCE_DIR}/libcrypto.a")
get_property(abc TARGET openssl-crypto-static PROPERTY IMPORTED_LOCATION )
add_dependencies(openssl-crypto-static project_openssl)

add_library(openssl-ssl-static STATIC IMPORTED)
#target_link_libraries(openssl-ssl-static INTERFACE dl)
set_property(TARGET openssl-ssl-static PROPERTY IMPORTED_LOCATION "${SOURCE_DIR}/libssl.a")
add_dependencies(openssl-ssl-static project_openssl)

