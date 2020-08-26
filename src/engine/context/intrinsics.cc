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

#include "engine/intrinsics.h"

#include "engine/version.h"
#include "host/libc-module.h"
#include "log/tracer.h"
#include "threads/spinlock.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "engine::IntrinsicContext-"

namespace engine {
static engine::SpinLock s_intrinsic_context_locker;
// static
std::shared_ptr<IntrinsicContext>
IntrinsicContext::GetInstance(uint32_t version) {
  std::lock_guard<engine::SpinLock> lock(s_intrinsic_context_locker);
  static std::shared_ptr<IntrinsicContext> intrinsic_context_(
      new IntrinsicContext(version));
  LOG_D("try to get IntrinsicContext for V%u (with existed version V%u)",
        version, intrinsic_context_->Version());
  if (X_UNLIKELY(intrinsic_context_->Version() != version)) {
    LOG_D("try to replace IntrinsicContext V%u with V%u",
          intrinsic_context_->Version(), version);
    intrinsic_context_ =
        std::shared_ptr<IntrinsicContext>(new IntrinsicContext(version));
  }
  return intrinsic_context_;
}

IntrinsicContext::IntrinsicContext(uint32_t version) : version_(version) {
  LOG_D("construct IntrinsicContext for V%u", version);
  X_AUTO_TRACE("construct IntrinsicContext::IntrinsicContext V%u", version_);
  env_ = engine::make_unique_ptr<engine::Environment>();
  host_module_ = env_->AppendHostModule("env");
  LibcModule::RegisterInstrinsics(this, version_);
}

engine::Environment *IntrinsicContext::GetEnvironment() const {
  return env_ ? env_.get() : nullptr;
}

bool IntrinsicContext::GetHostModule(
    std::vector<engine::HostModule *> &vec /*out*/) const {
  vec.emplace_back(host_module_);
  return vec.size() > 0;
}

IntrinsicContext::~IntrinsicContext() {
  env_ = nullptr;
  host_module_ = nullptr;
}

void IntrinsicContext::register_intrinsic_memory(const string_view &memory_name,
                                                 uint64_t initial,
                                                 uint64_t max) {
  Limits limits =
      max == kInvalidOffset ? Limits(initial) : Limits(initial, max);
  host_module_->AppendMemoryExport(memory_name, limits);
}

void IntrinsicContext::register_intrinsic_table(const string_view &table_name,
                                                uint64_t initial,
                                                uint64_t max) {
  Limits limits =
      max == kInvalidOffset ? Limits(initial) : Limits(initial, max);
  host_module_->AppendTableExport(table_name, limits);
}

} // namespace engine
