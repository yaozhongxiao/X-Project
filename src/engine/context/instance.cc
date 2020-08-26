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


 */

#include "engine/engine-instance.h"

#include "engine/engine-instance.h"
#include "log/log.h"
#include "log/tracer.h"

namespace engine {
// static
std::unique_ptr<EngineInstance>
EngineInstance::New(Context::EngineType engine_type) {
  X_AUTO_TRACE("EngineInstance::New");
  std::unique_ptr<EngineInstance> instance = nullptr;
  switch (engine_type) {
  case Context::EngineType::VM_INTERP:
    // instance = std::make_shared<InterpInstance>(engine_type);
    break;
  case Context::EngineType::VM_JIT:
    // TODO(zhongxiao.yzx) : support the jit instance
    break;
  default:
    LOG_E("%s @%s:%s", "create engine contenxt with unknow EngineType(0x%x)",
          __FILE__, __FUNCTION__);
    break;
  }
  return instance;
}

EngineInstance::EngineInstance(const Context::EngineType engine_type)
    : engine_type_(engine_type) {}

EngineInstance::~EngineInstance() {}

} // namespace engine
