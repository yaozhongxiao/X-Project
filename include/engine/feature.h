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

#ifndef X_INCLUDE_ENGINE_FEATURE_H_
#define X_INCLUDE_ENGINE_FEATURE_H_

/* =================================================================================
 *               variable       flag      default     description
 * ---------------------------------------------------------------------------------
 */
#define FOREACH_ENGINE_FEATURE(ENGINE_FEATURE)                                 \
  ENGINE_FEATURE(exceptions, "exceptions", false,                              \
                 "Experimental exception handling")                            \
  ENGINE_FEATURE(mutable_globals, "mutable-globals", true,                     \
                 "Support mutable globals")                                    \
  ENGINE_FEATURE(sat_float_to_int, "saturating-float-to-int", false,           \
                 "Saturate float")                                             \
  ENGINE_FEATURE(sign_extension, "sign-extension", false,                      \
                 "Sign-extension operators")                                   \
  ENGINE_FEATURE(simd, "simd", false, "SIMD support")                          \
  ENGINE_FEATURE(threads, "threads", false, "Threading support")               \
  ENGINE_FEATURE(multi_value, "multi-value", false, "Multi-value")             \
  ENGINE_FEATURE(tail_call, "tail-call", false, "Tail-call support")

namespace engine {

class Features {
public:
#define ENGINE_FEATURE(variable, flag, default_, help)                         \
  bool variable##_enabled() const { return variable##_enabled_; }              \
  void enable_##variable() { variable##_enabled_ = true; }                     \
  void disable_##variable() { variable##_enabled_ = false; }                   \
  void set_##variable##_enabled(bool value) { variable##_enabled_ = value; }
  // declare for each feature
  FOREACH_ENGINE_FEATURE(ENGINE_FEATURE)
#undef ENGINE_FEATURE

private:
#define ENGINE_FEATURE(variable, flag, default_, help)                         \
  bool variable##_enabled_ = default_;
  // declare for each feature
  FOREACH_ENGINE_FEATURE(ENGINE_FEATURE)
#undef ENGINE_FEATURE
};

} // namespace engine
#endif // X_INCLUDE_ENGINE_FEATURE_H_
