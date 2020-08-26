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
 */

#ifndef X_INCLUDE_RLIMIT_H_
#define X_INCLUDE_RLIMIT_H_

#include <stdint.h>

namespace engine {
#define CONTEXT_MEMORY_SIZE 320

#define MEM_LIMIT (16 * 1024 * 1024)
#define STACK_LIMIT (64 * 1024)
#define CALL_DEPTH_LIMIT 1024
#define CACHE_LIMIT 20

/**
 * RLimit(resource limits) is used to represent resource limits
 * for engine execution, which includes gas, memory, time elapsed
 * and so on.
 */
struct RLimit {
  static uint64_t mem_limit_;        // memory size limits：default value-64MB
  static uint64_t stack_limit_;      // stack size limits: default value-64k
  static uint64_t call_depth_limit_; // contract call_depth limits
  static uint64_t cache_limit_;      // engine context cache limits
};

} // namespace engine
#endif // X_INCLUDE_RLIMIT_H_
