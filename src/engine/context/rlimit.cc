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

#include "engine/rlimit.h"

namespace engine {
uint64_t RLimit::gas_limit_ = 0;
uint64_t RLimit::vm_mem_limit_ = VM_MEM_LIMIT; // default limits value 16MB
uint64_t RLimit::stack_limit_ =
    STACK_LIMIT; // stack size limits: default value 64k
uint64_t RLimit::call_depth_limit_ = CALL_DEPTH_LIMIT; // default value 1024
uint64_t RLimit::vm_cache_limit_ = VM_CACHE_LIMIT;     // default value 20

} // namespace engine
