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

#ifndef X_INCLUDE_ENGINE_CONTEXT_H_
#define X_INCLUDE_ENGINE_CONTEXT_H_

#include <stdint.h>

#include <memory>

#include "engine/engine.h"
#include "engine/version.h"

namespace engine {

class Context {
public:
  enum class RunMode {
    Quiet = 0,
    Verbose = 1,
  };

  enum class EngineType {
    VM_UNKNOW = 0x00,
    VM_INTERP = 0x10,
    VM_JIT = 0x20,
  };

  static std::shared_ptr<Context>
  New(uint32_t version = X_CURRENT_VERSION,
      EngineType engine_type = EngineType::VM_INTERP);

  Context(const EngineType engine_type = EngineType::VM_INTERP,
          uint32_t version = X_CURRENT_VERSION);

  virtual ~Context();

  ALWAYS_INLINE uint32_t Version() { return version_; }

  static constexpr unsigned max_memory_size = 33 * 1024 * 1024; // bytes
  static constexpr unsigned max_globals_size = 1024;            // bytes
  static constexpr unsigned max_table_elements = 1024;          // elements
  static constexpr unsigned max_call_depth = 1024;              // nested calls
  static constexpr unsigned max_code_size = 20 * 1024 * 1024;   // bytes
  static constexpr unsigned runtime_page_size = 64 * 1024;
  static constexpr uint32_t runtime_type = 0x6d736100; /*\0asm*/
  static constexpr uint32_t min_runtime_size = 8; // shortest module is 8 bytes

protected:
  virtual bool InitEnvironment(uint32_t version) = 0;

  EngineType engine_type_;
  uint32_t version_;
};

} // namespace engine
#endif // X_INCLUDE_ENGINE_CONTEXT_H_
