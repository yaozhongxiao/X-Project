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
 *
 */

#include <cinttypes>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "engine/context.h"
#include "engine/feature.h"
#include "engine/rlimit.h"
#include "engine/version.h"
#include "log/tracer.h"
#include "utils/util.h"

using namespace engine; // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "plugins::ext_runtime-"

namespace plugin {

ExtPlugin::ExtPlugin(ExtVMPtr &ext_vm)
    : vm_context_ptr_(nullptr), ext_vm_ptr_(ext_vm) {}

ExtPlugin::ExtPlugin() : vm_context_ptr_(nullptr), ext_vm_ptr_(nullptr) {}

ExtPlugin::~ExtPlugin() { vm_context_ptr_ = nullptr; }

bool ExtPlugin::SetVMContext(const VMContextPtr vm_context_ptr) {
  vm_context_ptr_ = vm_context_ptr;
  return true;
}

void ExtPlugin::SetCallMethod(const std::string &method,
                              const std::string &params) {
  LOG_D("%s [%s]", __func__, execute_method_.c_str());
  execute_method_ = method;
  method_params_ = params;
}

} // namespace plugin
