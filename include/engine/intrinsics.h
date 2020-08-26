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

#ifndef X_INCLUDE_ENGINE_INTRINSICS_H_
#define X_INCLUDE_ENGINE_INTRINSICS_H_

#include <cassert>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "engine/engine.h"
#include "types/types.h"
#include "utils/index-sequence.h"

namespace engine {
/*
 * "apply" underline implementation by call func(c[0]...c[N-1])
 * typename R         : decalre type of return value for the function signature
 * typename... Args   : decalre type of parameters for the function signature
 * typename... T      : declare type of the context（the class of the
 * class::method) typename Container : decalre type of input values for the
 * function call (could be any type that operator[] supported, such as
 *                       std::vector)
 * size_t... I        : indexing list of the contianer[0 ... N-1]
 */
template <typename R, typename... Args, typename T, typename C, size_t... I>
R apply_impl(R (*func)(Args...), T cxt, const C &c,
             engine::index_sequence<I...>) {
  return (*func)(cxt, c[I]...);
}

template <typename R, typename... Args, typename T, typename C, size_t... I>
R apply_impl(std::function<R(Args...)> &callback, T cxt, const C &c,
             engine::index_sequence<I...>) {
  return callback(cxt, c[I]...);
}
/*
 * apply "func"(with return value) on container "c"
 * typename R         : decalre type of return value for the function signature
 * typename... Args   : decalre type of parameters for the function signature
 * typename... T      : declare type of the context（the class of the
 * class::method) typename Container : decalre type of input values for the
 * function call (could be any type that operator[] supported, such as
 *                       std::vector)
 */
template <typename R, typename... Args, typename T, typename Container>
R apply(R (*func)(Args...), T cxt, Container c) {
  return apply_impl(func, cxt, c,
                    engine::make_index_sequence<sizeof...(Args)>());
}

/*
 * apply "func"(without return value) on container "c"
 * typename... Args   : decalre type of parameters for the function signature
 * typename Container : decalre type of input values for the function call
 *                      (could be any type that operator[] supported, such as
 *                       std::vector)
 */
template <typename... Args, typename T, typename Container>
void apply(void (*func)(Args...), T cxt, Container c) {
  apply_impl(func, cxt, c, engine::make_index_sequence<sizeof...(Args)>());
}

/*
 * intrisic callback wrapper(with return value)
 * typename R         : decalre type of return value for the function signature
 * typename... Args   : decalre type of parameters for the function signature
 */
template <typename R, typename... Args>
engine::Result apply_intrinsic(engine::Thread *thread,
                               const engine::FuncSignature *sig,
                               const TypedValues &args, TypedValues &results,
                               R (*callback)(Args...)) {
  R ret = apply_impl(callback, thread, args,
                     engine::make_index_sequence<sizeof...(Args) - 1>());
  // FIXME(zhongxiao.yzx): only support 1 return value
  results[0] = ret;
  return engine::Result::Ok;
}

template <typename R, typename... Args,
          engine::enable_if_t<!std::is_void<R>::value> * = nullptr>
engine::Result apply_intrinsic(engine::Thread *thread,
                               const engine::FuncSignature *sig,
                               const TypedValues &args, TypedValues &results,
                               std::function<R(Args...)> callback) {
  R ret = apply_impl(callback, thread, args,
                     engine::make_index_sequence<sizeof...(Args) - 1>());
  // FIXME(zhongxiao.yzx): only support 1 return value
  results[0] = ret;
  return engine::Result::Ok;
}
/*
 * overloaded intrisic callback wrapper(without return value)
 * typename... Args   : decalre type of parameters for the function signature
 */
template <typename... Args>
engine::Result apply_intrinsic(engine::Thread *thread,
                               const engine::FuncSignature *sig,
                               const TypedValues &args, TypedValues &results,
                               void (*callback)(Args...)) {
  apply_impl(callback, thread, args,
             engine::make_index_sequence<sizeof...(Args) - 1>());
  return engine::Result::Ok;
}

template <typename... Args>
engine::Result apply_intrinsic(engine::Thread *thread,
                               const engine::FuncSignature *sig,
                               const TypedValues &args, TypedValues &results,
                               std::function<void(Args...)> callback) {
  apply_impl(callback, thread, args,
             engine::make_index_sequence<sizeof...(Args) - 1>());
  return engine::Result::Ok;
}
/****************************************************************************
 *  macro to register webassembly memory, table ,globals, and functions
 ****************************************************************************/
/*
 * CONTEXT     : context instance maintain module named "MODULE_NAME"(reserved)
 * MODULE_NAME : module exporting the function, global, memory, tables
 * FUNC_NAME   : name of the function to be exported
 * FUNC_PTR    : the function body(implementation) of the FUNC_NAME
 * if we want to export abortStackOverflow from module "env", we
 * can following the following steps
 * 1. define the function body
 * void abortStackOverflow(int32_t args) {
 * }
 * 2. export env.abortStackOverflow
 * REGISTER_EXPORT_FUNCTION(Context, "env", "abortStackOverflow",
 * signature,abortStackOverflow)
 * see emscripten.cc
 *
 */
#define REGISTER_EXPORT_FUNCTION(CONTEXT, MODULE_NAME, FUNC_NAME, FUNC_SIG,    \
                                 FUNC_PTR)                                     \
  assert(std::string("env") == MODULE_NAME);                                   \
  CONTEXT->register_intrinsic_function<FUNC_SIG>(FUNC_NAME, FUNC_PTR);

#define REGISTER_EXPORT_GLOBAL(CONTEXT, MODULE_NAME, GLOBAL_NAME, MUTABLE,     \
                               VALUE)                                          \
  assert(std::string("env") == MODULE_NAME);                                   \
  CONTEXT->register_intrinsic_global(GLOBAL_NAME, VALUE, MUTABLE);

#define REGISTER_EXPORT_MEMORY(CONTEXT, MODULE_NAME, MEMORY_NAME, INITIAL,     \
                               MAX)                                            \
  assert(std::string("env") == MODULE_NAME);                                   \
  CONTEXT->register_intrinsic_memory(MEMORY_NAME, INITIAL, MAX);

#define REGISTER_EXPORT_TABLE(CONTEXT, MODULE_NAME, TABLE_NAME, INITIAL, MAX)  \
  assert(std::string("env") == MODULE_NAME);                                   \
  CONTEXT->register_intrinsic_table(TABLE_NAME, INITIAL, MAX);

/*
 * declare export fuction "Method" simply by
 * DECLARE_EXPORT_CLASS_METHOD(Method, Return, Args...)
 * such as:
 *   The export fuction CheckAccount has the signature
 *   bool (*CheckAccount)(int32_t id)
 *
 *  DECLARE_EXPORT_CLASS_METHOD(CheckAccount, bool, int32_t id)
 *  the macro actually define the following method
 *  bool CheckAccount(engine::Thread* context, int32_t id);
 */
#define DECLARE_EXPORT_CLASS_METHOD(METHOD, R, ...)                            \
  static R S_##METHOD(engine::engine::Thread *context, ##__VA_ARGS__);         \
  R METHOD(engine::engine::Thread *context, ##__VA_ARGS__)

/*
 * define export fuction "Method" simply by
 * DEFINE_EXPORT_CLASS_METHOD(Class, Method, Return, Args...)
 * such as:
 *   The export fuction CheckAccount has the signature
 *   bool (*CheckAccount)(int32_t id)
 *
 *  DEFINE_EXPORT_CLASS_METHOD(ExtPlugin, CheckAccount, bool, int32_t id)
 *  the macro actually define the following method
 *  bool ExtPlugin::CheckAccount(engine::Thread* context, int32_t id);
 */
#define DEFINE_EXPORT_CLASS_METHOD(CLASS, METHOD, R, ...)                      \
  R CLASS::METHOD(engine::engine::Thread *context, ##__VA_ARGS__)

#define DEFINE_EXPORT_CLASS_STATIC_METHOD(CLASS, METHOD, R, ...)               \
  R CLASS::S_##METHOD(engine::engine::Thread *context, ##__VA_ARGS__)

/*
 * register class method to binding with imported function in runtime module
 * CONTEXT     : context instance maintain module named "MODULE_NAME"
 * MODULE_NAME : module exporting the function, global, memory, tables
 * FUNC_NAME   : name of the function to be exported
 * INSTANCE    : the instance of the CLASS
 * CLASS,METHOD: the method binding with FUNC_NAME(CLASS::METHOD)
 * R(ARGS...)  : the signature of FUNC_NAME
 */
#define REGISTER_EXPORT_CLASS_METHOD(CONTEXT, MODULE_NAME, FUNC_NAME,          \
                                     INSTANCE, CLASS,                          \
                                     METHOD /*CLASS::METHOD*/, R, ...)         \
  {                                                                            \
    std::function<R(engine::engine::Thread *, ##__VA_ARGS__)> f_##METHOD =     \
        std::bind(                                                             \
            &CLASS::METHOD, INSTANCE,                                          \
            PLACE_HODLER_REPEAT(ARGS_COUNT_PLUGIN_1(__, ##__VA_ARGS__)));      \
    CONTEXT->register_intrinsic_function(MODULE_NAME, FUNC_NAME, f_##METHOD);  \
  }

/*
 * register class method to binding with imported function in runtime module
 * CONTEXT     : context instance maintain module named "MODULE_NAME"
 * MODULE_NAME : module exporting the function, global, memory, tables
 * FUNC_NAME   : name of the function to be exported
 * INSTANCE    : skipped
 * CLASS,METHOD: the method binding with FUNC_NAME(CLASS::METHOD)
 * R(ARGS...)  : the signature of FUNC_NAME
 */
#define REGISTER_EXPORT_CLASS_STATIC_METHOD(CONTEXT, MODULE_NAME, FUNC_NAME,   \
                                            INSTANCE, CLASS,                   \
                                            METHOD /*CLASS::METHOD*/, R, ...)  \
  {                                                                            \
    std::function<R(engine::engine::Thread *, ##__VA_ARGS__)> f_##METHOD =     \
        std::bind(&CLASS::S_##METHOD, PLACE_HODLER_REPEAT(ARGS_COUNT_PLUGIN_1( \
                                          __, ##__VA_ARGS__)));                \
    CONTEXT->register_intrinsic_function(MODULE_NAME, FUNC_NAME, f_##METHOD);  \
  }

/****************************************************************************
 *  macro to declare and define the non-class method intrinsics functions
 ****************************************************************************/
#define INTRINSIC_FUNCTION(NAMESPACE, FUNC_NAME) NAMESPACE::FUNC_NAME
#define INTRINSIC_FUNCTION_SIGNATURE(NAMESPACE, FUNC_NAME)                     \
  NAMESPACE::FUNC_NAME##_SIG

#define DECLARE_EXPORT_INTRINSIC_FUNCTION(NAMESPACE, FUNC_NAME, R, ARGS...)    \
  namespace NAMESPACE {                                                        \
  extern R FUNC_NAME(engine::Thread *context, ##ARGS);                         \
  }

#define DEFINE_EXPORT_INTRINSIC_FUNCTION(NAMESPACE, FUNC_NAME, R, ARGS...)     \
  namespace NAMESPACE {                                                        \
  using FUNC_NAME##_SIG = R(ARGS);                                             \
  R FUNC_NAME(engine::Thread *context, ##ARGS);                                \
  }                                                                            \
  R NAMESPACE::FUNC_NAME(engine::Thread *context, ##ARGS)

/****************************************************************************
 *  the underline implementation of IntrinsicContext for exports register
 ****************************************************************************/
class IntrinsicContext {
public:
  static std::shared_ptr<IntrinsicContext> GetInstance(uint32_t version);
  ~IntrinsicContext();

  engine::Environment *GetEnvironment() const;

  ALWAYS_INLINE uint32_t Version() { return version_; }

  // get HostModule in vec from IntrinsicContext
  bool GetHostModule(std::vector<engine::HostModule *> &vec /*out*/) const;

  template <typename Signature, typename FuncPtr>
  void register_intrinsic_function(const string_view &func_name,
                                   FuncPtr callback) {
    host_module_->AppendFuncExport(
        func_name, engine::function_signature_provider<Signature>::type(),
        [callback](engine::Thread *thread, const engine::HostFunc *func,
                   const engine::FuncSignature *sig, const TypedValues &args,
                   TypedValues &results) -> engine::Result {
          return apply_intrinsic(thread, sig, args, results, callback);
        });
  }

  template <typename T>
  void register_intrinsic_global(const string_view &global_name, T value,
                                 bool mut = false) {
    host_module_->AppendGlobalExport(global_name, mut, value);
  }

  void register_intrinsic_table(const string_view &table_name, uint64_t initial,
                                uint64_t max = kInvalidOffset);

  void register_intrinsic_memory(const string_view &memory_name,
                                 uint64_t initial,
                                 uint64_t max = kInvalidOffset);

private:
  explicit IntrinsicContext(uint32_t version);

  // host_module is not self-owned (create and maintained by env_)
  engine::HostModule *host_module_;
  std::unique_ptr<engine::Environment> env_;
  uint32_t version_;
};

} // namespace engine
#endif // X_INCLUDE_ENGINE_INTRINSICS_H_
