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

#ifndef X_INCLUDE_ENGINE_OPTIONS_H_
#define X_INCLUDE_ENGINE_OPTIONS_H_

#include <stdint.h>

#include "engine/version.h"
#include "utils/util.h"

namespace engine {
/**
 * Options is used to represent execution options for engine
 */
struct Options {
  explicit Options(uint64_t gas = 0, bool trace = false)
      : gas_value_(gas), trace_(trace) {}

  uint64_t gas_value_ = 0;
  uint32_t api_level_ = X_CURRENT_VERSION;
  bool trace_ = false;
  // engine binded features
  static Features features_;
};

} // namespace engine
#endif // X_INCLUDE_ENGINE_OPTIONS_H_
