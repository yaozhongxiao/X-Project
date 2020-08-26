/**
 * Copyright 2020 Develop Group Participants. All rights reserved.
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

#include "engine/context.h"

#include "engine/engine-context.h"
#include "log/log.h"
#include "log/tracer.h"

namespace engine {
static_assert(Context::max_memory_size % Context::runtime_page_size == 0,
              "maximum_linear_memory must be mulitple of runtime page size");
static_assert(Context::max_globals_size % 4 == 0,
              "maximum_mutable_globals must be mulitple of 4");
static_assert(Context::max_table_elements * 8 % 4096 == 0,
              "maximum_table_elements*8 must be mulitple of 4096");

// static
std::shared_ptr<Context> Context::New(uint32_t version,
                                      EngineType engine_type) {
  X_AUTO_TRACE("Context::New V%u", version);
  std::shared_ptr<Context> context = nullptr;
  switch (engine_type) {
  case EngineType::VM_INTERP:
    context = make_unique_ptr<InterpContext>(engine_type, version);
    break;
  case EngineType::VM_JIT:
    // TODO(zhongxiao.yzx) : support the jit context
    break;
  default:
    LOG_F("%s @%s:%s", "create engine contenxt with unknow EngineType(0x%x)",
          __FILE__, __FUNCTION__);
    break;
  }
  return context;
}

Context::Context(const EngineType engine_type, uint32_t version)
    : engine_type_(engine_type), version_(version) {}

Context::~Context() {}

} // namespace engine
