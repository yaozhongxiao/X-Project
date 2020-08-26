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

#ifndef X_INCLUDE_ENGINE_INSTANCE_H_
#define X_INCLUDE_ENGINE_INSTANCE_H_

#include <stdint.h>

#include <memory>

#include "engine/context.h"

namespace engine {

class EngineInstance {
public:
  static std::unique_ptr<EngineInstance>
  New(Context::EngineType engine_type = Context::EngineType::VM_INTERP);

  explicit EngineInstance(
      const Context::EngineType engine_type = Context::EngineType::VM_INTERP);
  virtual ~EngineInstance();

  virtual engine::ExecResult RunStartFunction() = 0;

  virtual engine::ExecResult
  RunExportByName(const string_view &name, const engine::TypedValues &args) = 0;
  virtual void
  RunAllExports(const Context::RunMode &verbose = Context::RunMode::Quiet) = 0;

  virtual engine::FuncSignature *
  FindExportFuncSignature(const string_view &name) = 0;
  inline Context::EngineType GetEngineType() { return engine_type_; }

private:
  Context::EngineType engine_type_;
};

} // namespace engine
#endif // X_INCLUDE_ENGINE_INSTANCE_H_
