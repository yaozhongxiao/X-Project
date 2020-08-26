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

#include "host/libc-module.h"

// try to get rid of the compile flag "-Wold-style-cast" for openssl
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic pop

#include "crypto/crypto.h"
#include "log/tracer.h"
#include "utils/util.h"
#include "vm/feature.h"
#include "vm/intrinsics.h"
#include "vm/version.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "myvm::LibcModule-"

namespace plugin {

// NOTE(X) : X_NULL represents invalid pointer
#define X_NULL 0
#define INT_MIN_ -2147483648 // 0x80000000
#define INT_MAX_ 2147483647  // 0x7fffffff
#define UINT_MAX_ 4294967295 // 0xffffffff
// the char count in string with null-termination
// #define STR_SIZE 4294967295  // 0xffffffff
#define WCHAR_SIZE 4
#define BYTES_TO_WCHAR_NUM(bytes) (bytes) >> 2
#define WCHAR_NUM_TO_BYTES(num) (num) << 2

#ifdef X_DEBUG
#define PRINT_BUFFER_SIZE (4 * 1024)
#else
#define PRINT_BUFFER_SIZE 512
#endif

// void (*abortStackOverflow)(int32_t args)
DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, abortStackOverflow, void,
                                 int32_t args) {
  LOG_D("abortStackOverflow callback with value %d", args);
}

// int32_t (*my_printf)(int32_t fmt,int32_t args)
DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_printf, int32_t,
                                 uint32_t fmt_offset, uint32_t arg_offset) {
  X_TRACE_BEGIN("%s", __func__);
  char *fmt = X_STATIC_POINTER_CAST(fmt_offset);
  char *param = X_STATIC_POINTER_CAST(arg_offset);
  if (X_UNLIKELY(!fmt || !param)) {
    LOG_E("%s access invalid memory", __func__);
    return 1;
  }

  uint64_t memory_size = X_LINEAR_MEMORY_SIZE();
  char *mem_base;
  context->GetMemoryBase(&mem_base);
  char buf[PRINT_BUFFER_SIZE];
  mem_vsnprintf(mem_base, memory_size, buf, sizeof(buf), fmt, param);
  LOG_I("%s\n", buf);
  X_TRACE_END("%s", __func__);
  return 0;
}

// Bypassing some libc interfaces and implementing them directly in the VM
// could greatly improve performance of wasm code.
DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_memcpy, uint32_t,
                                 uint32_t wasm_dst, uint32_t wasm_src,
                                 uint32_t size) {
  GAS_CONSUME(INTRINSIC_CALL_COST + GAS_COST_MEMORY_MODULUS * size);
  char *dst = X_STATIC_BUFFER_CAST(wasm_dst, size);
  const char *src = X_STATIC_BUFFER_CAST(wasm_src, size);
  if (X_LIKELY(dst && src)) {
    memcpy(dst, src, size);
  }
  return wasm_dst;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_wmemcpy, uint32_t,
                                 uint32_t wasm_dst, uint32_t wasm_src,
                                 uint32_t count) {
  size_t size = count * WCHAR_SIZE;
  GAS_CONSUME(INTRINSIC_CALL_COST + GAS_COST_MEMORY_MODULUS * size);
  char *dst = X_STATIC_BUFFER_CAST(wasm_dst, size);
  const char *src = X_STATIC_BUFFER_CAST(wasm_src, size);
  if (X_LIKELY(dst && src)) {
    wmemcpy(reinterpret_cast<wchar_t *>(dst),
            reinterpret_cast<const wchar_t *>(src), count);
  }
  return wasm_dst;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_memset, uint32_t,
                                 uint32_t wasm_dst, int32_t val,
                                 uint32_t size) {
  GAS_CONSUME(INTRINSIC_CALL_COST + GAS_COST_MEMORY_MODULUS * size);
  char *dst = X_STATIC_BUFFER_CAST(wasm_dst, size);
  if (X_LIKELY(dst)) {
    memset(dst, val, size);
  }
  return wasm_dst;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_wmemset, uint32_t,
                                 uint32_t wasm_dst, uint32_t val,
                                 uint32_t count) {
  size_t size = count * WCHAR_SIZE;
  GAS_CONSUME(INTRINSIC_CALL_COST + GAS_COST_MEMORY_MODULUS * size);
  char *dst = X_STATIC_BUFFER_CAST(wasm_dst, size);
  if (X_LIKELY(dst)) {
    wmemset(reinterpret_cast<wchar_t *>(dst), static_cast<wchar_t>(val), count);
  }
  return wasm_dst;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_memcmp, int32_t,
                                 uint32_t wasm_vl, uint32_t wasm_vr,
                                 uint32_t size) {
  GAS_CONSUME(INTRINSIC_CALL_COST + GAS_COST_MEMORY_MODULUS * size);
  const char *lhs = X_STATIC_BUFFER_CAST(wasm_vl, size);
  const char *rhs = X_STATIC_BUFFER_CAST(wasm_vr, size);
  if (X_LIKELY(lhs && rhs)) {
    return memcmp(lhs, rhs, size);
  }
  return -1;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_wmemcmp, int32_t,
                                 uint32_t wasm_vl, uint32_t wasm_vr,
                                 uint32_t count) {
  size_t size = count * WCHAR_SIZE;
  GAS_CONSUME(INTRINSIC_CALL_COST + GAS_COST_MEMORY_MODULUS * size);
  const char *vl = X_STATIC_BUFFER_CAST(wasm_vl, size);
  const char *vr = X_STATIC_BUFFER_CAST(wasm_vr, size);
  if (X_LIKELY(vl && vr)) {
    return wmemcmp(reinterpret_cast<const wchar_t *>(vl),
                   reinterpret_cast<const wchar_t *>(vr), count);
  }
  return -1;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_memmove, uint32_t,
                                 uint32_t wasm_dst, uint32_t wasm_src,
                                 uint32_t size) {
  GAS_CONSUME(INTRINSIC_CALL_COST + GAS_COST_MEMORY_MODULUS * size);
  char *dst = X_STATIC_BUFFER_CAST(wasm_dst, size);
  const char *src = X_STATIC_BUFFER_CAST(wasm_src, size);
  if (X_LIKELY(dst && src)) {
    memmove(dst, src, size);
  }
  return wasm_dst;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_wmemmove, uint32_t,
                                 uint32_t wasm_dst, uint32_t wasm_src,
                                 uint32_t count) {
  size_t size = count * WCHAR_SIZE;
  GAS_CONSUME(INTRINSIC_CALL_COST + GAS_COST_MEMORY_MODULUS * size);
  char *dst = X_STATIC_BUFFER_CAST(wasm_dst, size);
  const char *src = X_STATIC_BUFFER_CAST(wasm_src, size);
  if (X_LIKELY(dst && src)) {
    wmemmove(reinterpret_cast<wchar_t *>(dst),
             reinterpret_cast<const wchar_t *>(src), count);
  }
  return wasm_dst;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_memchr, uint32_t, /* "void*" */
                                 uint32_t mem_offset, int ch, uint32_t count) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  uint32_t max_size = X_LINEAR_MEMORY_SIZE() - mem_offset;
  max_size = max_size > count ? count : max_size;
  char *buffer = X_STATIC_BUFFER_CAST(mem_offset, max_size);
  if (X_UNLIKELY(!buffer)) {
    return X_NULL;
  }
  void *ret = memchr(buffer, ch, max_size);
  if (X_UNLIKELY(!ret)) {
    return X_NULL;
  }
  return X_LINEAR_MEMORY_OFFSET(static_cast<char *>(ret));
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_strlen, uint32_t, uint32_t str) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  const char *str_ptr = X_STATIC_POINTER_CAST(str);
  if (X_LIKELY(str_ptr)) {
    return strlen(str_ptr);
  }
  return 0; // by design
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_strchrnul, uint32_t,
                                 uint32_t str, int ch) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  const char *str_ptr = X_STATIC_POINTER_CAST(str);
  if (X_UNLIKELY(!str_ptr)) {
    return X_NULL;
  }
  const char *ret = strchr(str_ptr, ch);
  if (X_UNLIKELY(!ret)) {
    // point to tail '\0'
    ret = str_ptr + strlen(str_ptr);
  }
  return X_LINEAR_MEMORY_OFFSET(ret);
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_strcmp, int32_t,
                                 uint32_t wasm_sl, uint32_t wasm_sr) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  const char *sl = X_STATIC_POINTER_CAST(wasm_sl);
  const char *sr = X_STATIC_POINTER_CAST(wasm_sr);
  if (X_LIKELY(sl && sr)) {
    return strcmp(sl, sr);
  }
  return -1;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_strncmp, int, uint32_t wasm_sl,
                                 uint32_t wasm_sr, uint32_t count) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  const char *sl = X_STATIC_POINTER_CAST(wasm_sl);
  const char *sr = X_STATIC_POINTER_CAST(wasm_sr);
  if (X_LIKELY(sl && sr)) {
    return strncmp(sl, sr, count);
  }
  return -1; // by design
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_strcpy, uint32_t,
                                 uint32_t wasm_dst, uint32_t wasm_src) {
  const char *src = X_STATIC_POINTER_CAST(wasm_src);
  if (X_UNLIKELY(!src)) {
    return wasm_dst;
  }
  // copy src including '\0'
  size_t size = strlen(src) + 1;
  GAS_CONSUME(INTRINSIC_CALL_COST + GAS_COST_MEMORY_MODULUS * size);
  char *dst = X_STATIC_BUFFER_CAST(wasm_dst, size);
  if (X_LIKELY(dst)) {
    strncpy(dst, src, size);
  }
  return wasm_dst;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_strncpy, uint32_t,
                                 uint32_t wasm_dst, uint32_t wasm_src,
                                 uint32_t count) {
  GAS_CONSUME(INTRINSIC_CALL_COST + GAS_COST_MEMORY_MODULUS * count);
  char *src = X_STATIC_POINTER_CAST(wasm_src);
  char *dst = X_STATIC_BUFFER_CAST(wasm_dst, count);
  if (X_UNLIKELY(!src || !dst)) {
    return wasm_dst;
  }
  strncpy(dst, src, count);
  return wasm_dst;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_strncat, uint32_t,
                                 uint32_t wasm_dst, uint32_t wasm_src,
                                 uint32_t count) {
  const char *src = X_STATIC_POINTER_CAST(wasm_src);
  char *dst = X_STATIC_POINTER_CAST(wasm_dst);
  if (X_UNLIKELY(!src || !dst)) {
    return wasm_dst;
  }
  size_t src_size = strlen(src);
  src_size = src_size > count ? count : src_size;
  size_t dst_size = strlen(dst) + 1 + src_size;
  GAS_CONSUME(INTRINSIC_CALL_COST + GAS_COST_MEMORY_MODULUS * src_size);
  dst = X_STATIC_BUFFER_CAST(wasm_dst, dst_size);
  if (X_LIKELY(dst)) {
    strncat(dst, src, src_size);
  }
  return wasm_dst;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_wmemchr, uint32_t,
                                 uint32_t wasm_src, uint32_t wch,
                                 uint32_t count) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  uint32_t max_size = X_LINEAR_MEMORY_SIZE() - wasm_src;
  uint32_t count_size = WCHAR_NUM_TO_BYTES(count);
  max_size = max_size > count_size ? count_size : max_size;
  const char *src = X_STATIC_BUFFER_CAST(wasm_src, max_size);
  if (X_UNLIKELY(!src)) {
    return X_NULL;
  }
  count = BYTES_TO_WCHAR_NUM(max_size);
  const wchar_t *wstr = wmemchr(reinterpret_cast<const wchar_t *>(src),
                                static_cast<wchar_t>(wch), count);
  if (X_UNLIKELY(!wstr)) {
    return X_NULL;
  }
  const char *str = reinterpret_cast<const char *>(wstr);
  return X_LINEAR_MEMORY_OFFSET(str);
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_wcslen, uint32_t,
                                 uint32_t wasm_wcs) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  const char *wstr = X_STATIC_POINTER_CAST(wasm_wcs);
  if (X_LIKELY(wstr)) {
    return wcslen(reinterpret_cast<const wchar_t *>(wstr));
  }
  return 0;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_wcschr, uint32_t,
                                 uint32_t wasm_src, uint32_t wch) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  const char *src = X_STATIC_POINTER_CAST(wasm_src);
  const wchar_t *wstr =
      wcschr(reinterpret_cast<const wchar_t *>(src), static_cast<wchar_t>(wch));
  if (X_UNLIKELY(!wstr)) {
    return X_NULL;
  }
  const char *str = reinterpret_cast<const char *>(wstr);
  return X_LINEAR_MEMORY_OFFSET(str);
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_wcscmp, int32_t,
                                 uint32_t wasm_vl, uint32_t wasm_vr) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  const char *vl = X_STATIC_POINTER_CAST(wasm_vl);
  const char *vr = X_STATIC_POINTER_CAST(wasm_vr);
  if (X_LIKELY(vl && vr)) {
    return wcscmp(reinterpret_cast<const wchar_t *>(vl),
                  reinterpret_cast<const wchar_t *>(vr));
  }
  return -1;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_wcsncmp, int32_t,
                                 uint32_t wasm_wl, uint32_t wasm_wr,
                                 uint32_t count) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  const char *wl = X_STATIC_POINTER_CAST(wasm_wl);
  const char *wr = X_STATIC_POINTER_CAST(wasm_wr);
  if (X_LIKELY(wl && wr)) {
    return wcsncmp(reinterpret_cast<const wchar_t *>(wl),
                   reinterpret_cast<const wchar_t *>(wr), count);
  }
  return -1;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_wcscpy, uint32_t,
                                 uint32_t wasm_dst, uint32_t wasm_src) {
  const char *src = X_STATIC_POINTER_CAST(wasm_src);
  if (X_UNLIKELY(!src)) {
    return wasm_dst;
  }
  const wchar_t *wsrc = reinterpret_cast<const wchar_t *>(src);
  // copy including L'\0'
  size_t count = wcslen(wsrc) + 1;
  GAS_CONSUME(INTRINSIC_CALL_COST +
              GAS_COST_MEMORY_MODULUS * count * WCHAR_SIZE);
  char *dst = X_STATIC_BUFFER_CAST(wasm_dst, count * WCHAR_SIZE);
  if (X_LIKELY(dst)) {
    wcsncpy(reinterpret_cast<wchar_t *>(dst), wsrc, count);
  }
  return wasm_dst;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_wcsncpy, uint32_t,
                                 uint32_t wasm_dst, uint32_t wasm_src,
                                 uint32_t count) {
  size_t size = count * WCHAR_SIZE;
  GAS_CONSUME(INTRINSIC_CALL_COST + GAS_COST_MEMORY_MODULUS * size);
  const char *src = X_STATIC_POINTER_CAST(wasm_src);
  char *dst = X_STATIC_BUFFER_CAST(wasm_dst, size);
  if (X_UNLIKELY(!src || !dst)) {
    return wasm_dst;
  }
  wcsncpy(reinterpret_cast<wchar_t *>(dst),
          reinterpret_cast<const wchar_t *>(src), count);
  return wasm_dst;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_wcsncat, uint32_t,
                                 uint32_t wasm_dst, uint32_t wasm_src,
                                 uint32_t count) {
  wchar_t *dst = reinterpret_cast<wchar_t *>(X_STATIC_POINTER_CAST(wasm_dst));
  wchar_t *src = reinterpret_cast<wchar_t *>(X_STATIC_POINTER_CAST(wasm_src));
  if (X_UNLIKELY(!src || !dst)) {
    return wasm_dst;
  }
  size_t src_count = wcslen(src);
  src_count = src_count > count ? count : src_count;
  size_t dst_count = wcslen(dst) + 1 + src_count;
  GAS_CONSUME(INTRINSIC_CALL_COST +
              GAS_COST_MEMORY_MODULUS * src_count * WCHAR_SIZE);
  dst = reinterpret_cast<wchar_t *>(
      X_STATIC_BUFFER_CAST(wasm_dst, dst_count * WCHAR_SIZE));
  if (X_LIKELY(dst)) {
    wcsncat(dst, src, src_count); // it actually copy src_count
  }
  return wasm_dst;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_strtol, int32_t, uint32_t str,
                                 uint32_t str_end, uint32_t base) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  long result = 0; // NOLINT
  const char *str_ptr = X_STATIC_POINTER_CAST(str);
  if (X_UNLIKELY(!str_ptr)) {
    LOG_W("try to strtol with invalid string");
    return result;
  }
  char *end_ptr = nullptr;
  result = strtol(str_ptr, &end_ptr, base);
  if (end_ptr && X_NULL != str_end) {
    // end_ptr will pointer to start_ptr while unexpected value got
    uint32_t offset = str;
    char *str_end_ptr = X_STATIC_BUFFER_CAST(str_end, sizeof(int32_t));
    if (str_end_ptr) {
      if (X_LIKELY(str_ptr != end_ptr)) {
        offset = X_LINEAR_MEMORY_OFFSET(end_ptr);
      }
      uint32_t *str_end_ptr_ptr = reinterpret_cast<uint32_t *>(str_end_ptr);
      *str_end_ptr_ptr = offset;
    }
  }
  return (result > INT_MAX_) ? INT_MAX_
                             : (result < INT_MIN_ ? INT_MIN_ : result);
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_strtoul, uint32_t, uint32_t str,
                                 uint32_t str_end, uint32_t base) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  uint32_t result = 0;
  const char *str_ptr = X_STATIC_POINTER_CAST(str);
  if (X_UNLIKELY(!str_ptr)) {
    LOG_W("try to strtoul with invalid string");
    return result;
  }
  char *end_ptr = nullptr;
  int64_t mid_result = strtoul(str_ptr, &end_ptr, base);
  if (end_ptr && X_NULL != str_end) {
    // end_ptr will pointer to start_ptr while unexpected value got
    uint32_t offset = str;
    char *str_end_ptr = X_STATIC_BUFFER_CAST(str_end, sizeof(int32_t));
    if (str_end_ptr) {
      if (X_LIKELY(str_ptr != end_ptr)) {
        offset = X_LINEAR_MEMORY_OFFSET(end_ptr);
      }
      uint32_t *str_end_ptr_ptr = reinterpret_cast<uint32_t *>(str_end_ptr);
      *str_end_ptr_ptr = offset;
    }
  }
  result = (mid_result > UINT_MAX_)
               ? UINT_MAX_
               : (mid_result < (-UINT_MAX_) ? UINT_MAX_ : mid_result);
  return result;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_strtoll, int64_t, uint32_t str,
                                 uint32_t str_end, uint32_t base) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  int64_t result = 0;
  const char *str_ptr = X_STATIC_POINTER_CAST(str);
  if (X_UNLIKELY(!str_ptr)) {
    LOG_W("try to strtoll with invalid string");
    return result;
  }
  char *end_ptr = nullptr;
  result = strtoll(str_ptr, &end_ptr, base);
  if (end_ptr && X_NULL != str_end) {
    // end_ptr will pointer to start_ptr while unexpected value got
    uint32_t offset = str;
    char *str_end_ptr = X_STATIC_BUFFER_CAST(str_end, sizeof(int32_t));
    if (str_end_ptr) {
      if (X_LIKELY(str_ptr != end_ptr)) {
        offset = X_LINEAR_MEMORY_OFFSET(end_ptr);
      }
      uint32_t *str_end_ptr_ptr = reinterpret_cast<uint32_t *>(str_end_ptr);
      *str_end_ptr_ptr = offset;
    }
  }
  return result;
}

DEFINE_EXPORT_INTRINSIC_FUNCTION(intrinsic, my_strtoull, uint64_t, uint32_t str,
                                 uint32_t str_end, uint32_t base) {
  GAS_CONSUME(INTRINSIC_CALL_COST);
  uint64_t result = 0;
  const char *str_ptr = X_STATIC_POINTER_CAST(str);
  if (X_UNLIKELY(!str_ptr)) {
    LOG_W("try to strtoull with invalid string");
    return result;
  }
  char *end_ptr = nullptr;
  result = strtoull(str_ptr, &end_ptr, base);
  if (end_ptr && X_NULL != str_end) {
    // end_ptr will pointer to start_ptr while unexpected value got
    uint32_t offset = str;
    char *str_end_ptr = X_STATIC_BUFFER_CAST(str_end, sizeof(int32_t));
    if (str_end_ptr) {
      if (X_LIKELY(str_ptr != end_ptr)) {
        offset = X_LINEAR_MEMORY_OFFSET(end_ptr);
      }
      uint32_t *str_end_ptr_ptr = reinterpret_cast<uint32_t *>(str_end_ptr);
      *str_end_ptr_ptr = offset;
    }
  }
  return result;
}

// static
void LibcModule::RegisterInstrinsics(IntrinsicContext *context,
                                     int32_t version) {
  LOG_D("try to register instrinsics for api-level(%u)", version);
  X_AUTO_TRACE("LibcModule::RegisterInstrinsics V%u", version);
  if (!context) {
    LOG_E("LibcModule try to register with invalid IntrinsicContext!");
    return;
  }
  // TODO(zhongxiao.yzx) : add export intrinsics as templates
  REGISTER_EXPORT_GLOBAL(context, "env", "memoryBase", false,
                         static_cast<wasm::i32>(1024));
  REGISTER_EXPORT_GLOBAL(context, "env", "tableBase", false,
                         static_cast<wasm::i32>(0));
  REGISTER_EXPORT_GLOBAL(context, "env", "STACKTOP", false,
                         static_cast<wasm::i32>(0));
  REGISTER_EXPORT_GLOBAL(context, "env", "STACK_MAX", false,
                         static_cast<wasm::i32>(0));

  // register intrinsic memory
  // NOTE(zhongxiao.yzx) : webassembly code generated from mycdt contains
  // self-defined memory and the webassembly mvp only support only one linear
  // memory, thus, we try to remove the system intrinsic linear memory so as
  // to accelerate the vm startup.
  // X_TRACE_BEGIN("register intrinsic memory");
  // REGISTER_EXPORT_MEMORY(context, "env", "memory", 256, 256)
  // X_TRACE_END("register intrinsic memory");

  // register intrinsic table
  REGISTER_EXPORT_TABLE(context, "env", "table", 0, 0)

  // register intrinsic functions
  REGISTER_EXPORT_FUNCTION(
      context, "env", "abortStackOverflow",
      INTRINSIC_FUNCTION_SIGNATURE(intrinsic, abortStackOverflow),
      INTRINSIC_FUNCTION(intrinsic, abortStackOverflow));

  REGISTER_EXPORT_FUNCTION(context, "env", "print",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_printf),
                           INTRINSIC_FUNCTION(intrinsic, my_printf));

  REGISTER_EXPORT_FUNCTION(context, "env", "memcpy",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_memcpy),
                           INTRINSIC_FUNCTION(intrinsic, my_memcpy));
  REGISTER_EXPORT_FUNCTION(context, "env", "wmemcpy",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_wmemcpy),
                           INTRINSIC_FUNCTION(intrinsic, my_wmemcpy));
  REGISTER_EXPORT_FUNCTION(context, "env", "memset",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_memset),
                           INTRINSIC_FUNCTION(intrinsic, my_memset));
  REGISTER_EXPORT_FUNCTION(context, "env", "wmemset",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_wmemset),
                           INTRINSIC_FUNCTION(intrinsic, my_wmemset));
  REGISTER_EXPORT_FUNCTION(context, "env", "memcmp",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_memcmp),
                           INTRINSIC_FUNCTION(intrinsic, my_memcmp));
  REGISTER_EXPORT_FUNCTION(context, "env", "wmemcmp",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_wmemcmp),
                           INTRINSIC_FUNCTION(intrinsic, my_wmemcmp));
  REGISTER_EXPORT_FUNCTION(context, "env", "memmove",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_memmove),
                           INTRINSIC_FUNCTION(intrinsic, my_memmove));
  REGISTER_EXPORT_FUNCTION(context, "env", "wmemmove",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_wmemmove),
                           INTRINSIC_FUNCTION(intrinsic, my_wmemmove));

  X_API_LEVEL_BEGIN(V01070)
  REGISTER_EXPORT_FUNCTION(context, "env", "memchr",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_memchr),
                           INTRINSIC_FUNCTION(intrinsic, my_memchr));
  REGISTER_EXPORT_FUNCTION(context, "env", "strlen",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_strlen),
                           INTRINSIC_FUNCTION(intrinsic, my_strlen));
  REGISTER_EXPORT_FUNCTION(
      context, "env", "__strchrnul",
      INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_strchrnul),
      INTRINSIC_FUNCTION(intrinsic, my_strchrnul));
  REGISTER_EXPORT_FUNCTION(context, "env", "strcmp",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_strcmp),
                           INTRINSIC_FUNCTION(intrinsic, my_strcmp));
  REGISTER_EXPORT_FUNCTION(context, "env", "strncmp",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_strncmp),
                           INTRINSIC_FUNCTION(intrinsic, my_strncmp));
  REGISTER_EXPORT_FUNCTION(context, "env", "strcpy",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_strcpy),
                           INTRINSIC_FUNCTION(intrinsic, my_strcpy));
  REGISTER_EXPORT_FUNCTION(context, "env", "strncpy",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_strncpy),
                           INTRINSIC_FUNCTION(intrinsic, my_strncpy));
  REGISTER_EXPORT_FUNCTION(context, "env", "strncat",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_strncat),
                           INTRINSIC_FUNCTION(intrinsic, my_strncat));
  REGISTER_EXPORT_FUNCTION(context, "env", "wmemchr",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_wmemchr),
                           INTRINSIC_FUNCTION(intrinsic, my_wmemchr));
  REGISTER_EXPORT_FUNCTION(context, "env", "wcslen",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_wcslen),
                           INTRINSIC_FUNCTION(intrinsic, my_wcslen));
  REGISTER_EXPORT_FUNCTION(context, "env", "wcschr",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_wcschr),
                           INTRINSIC_FUNCTION(intrinsic, my_wcschr));
  REGISTER_EXPORT_FUNCTION(context, "env", "wcscmp",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_wcscmp),
                           INTRINSIC_FUNCTION(intrinsic, my_wcscmp));
  REGISTER_EXPORT_FUNCTION(context, "env", "wcsncmp",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_wcsncmp),
                           INTRINSIC_FUNCTION(intrinsic, my_wcsncmp));
  REGISTER_EXPORT_FUNCTION(context, "env", "wcscpy",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_wcscpy),
                           INTRINSIC_FUNCTION(intrinsic, my_wcscpy));
  REGISTER_EXPORT_FUNCTION(context, "env", "wcsncpy",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_wcsncpy),
                           INTRINSIC_FUNCTION(intrinsic, my_wcsncpy));
  REGISTER_EXPORT_FUNCTION(context, "env", "wcsncat",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_wcsncat),
                           INTRINSIC_FUNCTION(intrinsic, my_wcsncat));

  REGISTER_EXPORT_FUNCTION(context, "env", "strtol",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_strtol),
                           INTRINSIC_FUNCTION(intrinsic, my_strtol));
  REGISTER_EXPORT_FUNCTION(context, "env", "strtoul",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_strtoul),
                           INTRINSIC_FUNCTION(intrinsic, my_strtoul));
  REGISTER_EXPORT_FUNCTION(context, "env", "strtoll",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_strtoll),
                           INTRINSIC_FUNCTION(intrinsic, my_strtoll));
  REGISTER_EXPORT_FUNCTION(context, "env", "strtoull",
                           INTRINSIC_FUNCTION_SIGNATURE(intrinsic, my_strtoull),
                           INTRINSIC_FUNCTION(intrinsic, my_strtoull));
  X_API_LEVEL_END(V01070)
}

} // namespace plugin
