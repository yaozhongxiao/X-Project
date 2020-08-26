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

#ifndef X_LIBRARIES_BASE_INCLUDE_CRYPTO_CRYPTO_H_
#define X_LIBRARIES_BASE_INCLUDE_CRYPTO_CRYPTO_H_

#include <stdint.h>

#include <string>
#include <vector>

namespace base {

void base64Encode(char *input_data, uint32_t in_len, char *output_data,
                  uint32_t &out_len);

bool base64Decode(char *input_data, uint32_t in_len, char *output_data,
                  uint32_t &out_len);

} // namespace base
#endif // X_LIBRARIES_BASE_INCLUDE_CRYPTO_CRYPTO_H_
