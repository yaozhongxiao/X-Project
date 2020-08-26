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

bool PluginRegister::InjectPluginCallback(uint32_t version) {
  LOG_D("try to inject external callback with api-level(%u) into engine",
        version);
  X_AUTO_TRACE("PluginRegister::InjectPluginCallback V[%u]", version);
  InterpContext *engine_context = dynamic_cast<InterpContext *>(context_.get());
  X_API_LEVEL_BEGIN(V0001)
  REGISTER_EXPORT_CLASS_STATIC_METHOD(engine_context, "env", "GetFileName",
                                      nullptr, ExtPlugin, GetFileName, int32_t,
                                      uint64_t, uint32_t, uint32_t);

  REGISTER_EXPORT_CLASS_STATIC_METHOD(engine_context, "env", "GetFileLength",
                                      nullptr, ExtPlugin, GetFileLength,
                                      uint64_t);
  X_API_LEVEL_END(V0001)

  return true;
}

} // namespace plugin
