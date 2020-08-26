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

#ifndef X_INCLUDE_VM_EXT_CONTEXT_H_
#define X_INCLUDE_VM_EXT_CONTEXT_H_

#include <stdint.h>

#include "engine/version.h"

namespace engine {
/*
 * class "ExtContext" works as the base class for external interface
 * which handler the callback from engine::Thread
 */
class ExtContext {
public:
  virtual ~ExtContext() {}
};

} // namespace engine
#endif // X_INCLUDE_VM_EXT_CONTEXT_H_
