/**
 * Copyright 2020 Develop Group Participants All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0(the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author : dongwei.ldw@antfin.com

 */

#ifndef X_INCLUDE_VERSION_H_
#define X_INCLUDE_VERSION_H_

#include <stdint.h>

namespace engine {

// v 0       1       04      0
//  major   minor   release patch
#define FOREACH_VERSION(VERSION_DELCARER)                                      \
  VERSION_DELCARER(V01040 = 0, "VM-V01040")                                    \
  VERSION_DELCARER(V01050 = 1, "VM-V01050")                                    \
  VERSION_DELCARER(V01060 = 2, "VM-V01060")                                    \
  VERSION_DELCARER(V01070 = 3, "VM-V01070")                                    \
  VERSION_DELCARER(V01080 = 4, "VM-V01080")                                    \
  VERSION_DELCARER(V01090 = 5, "VM-V01090")                                    \
  VERSION_DELCARER(V01100 = 6, "VM-V01100")

enum XVersion {
#define VERSION_DELCARER(name, desc) name,
  FOREACH_VERSION(VERSION_DELCARER)
#undef VERSION_DELCARER
      VMAX
};

// NOTE(engine) : must update the "X_CURRENT_VERSION" while
// update block_version caused for engine compatibility
#define X_CURRENT_VERSION V01100

/**
 *   block_version is in the following format
 *   |      2 bytes      |     6 bytes      |
 *   +-------------------+------------------+
 *   |     engine_version  |     reserverd    |
 *   +-------------------+------------------+
 */
#define X_VERSION(block_version)                                               \
  (0xFFFF & (static_cast<uint64_t>(block_version) >> 48))

#define BLOCK_VERSION(version, reserved)                                       \
  (((0xFFFF & (static_cast<uint64_t>(version))) << 48) |                       \
   ((~(0xFFFFull << 48)) & reserved))

#define VERSION_EXCEPTION(V) if (V > X_CURRENT_VERSION)

#define X_API_LEVEL_BEGIN(V) if (version >= (V)) {
#define X_API_LEVEL_END(V) }

extern const char *kVersionDesc[XVersion::VMAX];

} // namespace engine
#endif // X_INCLUDE_VERSION_H_
