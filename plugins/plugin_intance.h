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

#ifndef X_PLUGINS_MYCHAIN_INCLUDE_PLUGINS_VM_PLUGIN_WASM_EXT_WASM_H_
#define X_PLUGINS_MYCHAIN_INCLUDE_PLUGINS_VM_PLUGIN_WASM_EXT_WASM_H_

#include <memory>
#include <string>
#include <vector>

#include "engine/ext-context.h"
#include "engine/intrinsics.h"
#include "plugins/vm_plugin/runtime/runtime.h"
#include "utils/util.h"

namespace plugin {
using engine::IstreamOffset;
using engine::engine::Thread;
class VMDelegate;

#define RESULT_SUC 0
#define RESULT_FAIL 1
#define RESULT_NORMALIZE(result)                                               \
  (result == RESULT_SUC ? RESULT_SUC : RESULT_FAIL)

class ExtPlugin : public virtual engine::ExtContext {
public:
  ExtPlugin();
  explicit ExtPlugin(ExtVMPtr &ext_vm);
  virtual ~ExtPlugin();

  ExtVMPtr GetExtVM() { return ext_vm_ptr_; }

  bool SetVMContext(const VMContextPtr vm_context_ptr);
  void SetCallMethod(const std::string &method, const std::string &params);

  // check develop docs for detail signatures
  // https://yuque.antfin-inc.com/uf1pgz/yfkeyo/ff3a2v
  DECLARE_EXPORT_CLASS_METHOD(GetBlockHash, int32_t, uint64_t block_num,
                              uint32_t hash_data /*out*/,
                              uint32_t hash_len_offset /*out*/);

  DECLARE_EXPORT_CLASS_METHOD(GetBlockNumber, uint64_t);

  DECLARE_EXPORT_CLASS_METHOD(GetBlockTimeStamp, uint64_t);

  DECLARE_EXPORT_CLASS_METHOD(Log, int32_t, uint32_t, uint32_t, uint32_t,
                              uint32_t);

  DECLARE_EXPORT_CLASS_METHOD(SetStorage, int32_t, uint32_t key_data,
                              uint32_t key_len, uint32_t value_data,
                              uint32_t value_len);

  DECLARE_EXPORT_CLASS_METHOD(GetStorageSize, int32_t, uint32_t key_data,
                              uint32_t key_len, uint32_t value_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(GetStorage, int32_t, uint32_t key_data,
                              uint32_t key_len, uint32_t value_data,
                              uint32_t value_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(DeleteStorage, int32_t, uint32_t key_data,
                              uint32_t key_len);

  DECLARE_EXPORT_CLASS_METHOD(GetCode, int32_t, uint32_t id_code,
                              uint32_t id_len, uint32_t code_data,
                              uint32_t code_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(GetDataSize, uint32_t);

  DECLARE_EXPORT_CLASS_METHOD(GetData, int32_t, uint32_t data,
                              uint32_t len_offset);

  DECLARE_EXPORT_CLASS_METHOD(GetTxHash, int32_t, uint32_t tx_hash_data,
                              uint32_t tx_hash_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(GetGas, uint64_t);

  DECLARE_EXPORT_CLASS_METHOD(GetValue, uint64_t);

  DECLARE_EXPORT_CLASS_METHOD(GetSender, int32_t, uint32_t sender_data,
                              uint32_t sender_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(GetOrigin, int32_t, uint32_t origin_data,
                              uint32_t origin_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(CheckAccount, bool, uint32_t id_data,
                              uint32_t id_len);

  DECLARE_EXPORT_CLASS_METHOD(GetAccountStatus, int32_t, uint32_t id_data,
                              uint32_t id_len, uint32_t status_offset);

  DECLARE_EXPORT_CLASS_METHOD(GetCodeHash, int32_t, uint32_t id_data,
                              uint32_t id_len, uint32_t hash_data,
                              uint32_t hash_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(GetAuthMap, int32_t, uint32_t id_data,
                              uint32_t id_len, uint32_t auth_data,
                              uint32_t auth_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(GetBalance, int32_t, uint32_t id_data,
                              uint32_t id_len, uint32_t balance_offset);

  DECLARE_EXPORT_CLASS_METHOD(TransferBalance, int32_t, uint32_t to_data,
                              uint32_t to_len, uint64_t value);

  DECLARE_EXPORT_CLASS_METHOD(MyAbort, int32_t, uint32_t exception_data,
                              uint32_t exception_len);

  DECLARE_EXPORT_CLASS_METHOD(Result, int32_t, uint32_t output_data,
                              uint32_t output_len);

  DECLARE_EXPORT_CLASS_METHOD(GetSelf, int32_t, uint32_t self_data,
                              uint32_t self_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(GetRecoverKey, int32_t, uint32_t id_data,
                              uint32_t id_len, uint32_t recover_key_data,
                              uint32_t recover_key_len);

  DECLARE_EXPORT_CLASS_METHOD(ReadInterfaceName, int32_t, uint32_t name_data,
                              uint32_t name_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(ReadInterfaceNameSize, uint32_t);

  DECLARE_EXPORT_CLASS_METHOD(ReadInterfaceParams, int32_t,
                              uint32_t params_data, uint32_t params_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(ReadInterfaceParamsSize, uint32_t);

  DECLARE_EXPORT_CLASS_METHOD(CallContract, int32_t, uint32_t id_data,
                              uint32_t id_len, uint32_t method_data,
                              uint32_t method_len, uint64_t val, uint64_t gas,
                              uint32_t args_data, uint32_t args_len);

  DECLARE_EXPORT_CLASS_METHOD(DelegateCall, int32_t, uint32_t id_data,
                              uint32_t id_len, uint32_t method_data,
                              uint32_t method_len, uint32_t args_data,
                              uint32_t args_len);

  DECLARE_EXPORT_CLASS_METHOD(VerifyCommitment, bool, uint32_t type,
                              uint32_t index, uint32_t to_data, uint32_t to_len,
                              uint32_t enc_data, uint32_t enc_len);

  DECLARE_EXPORT_CLASS_METHOD(VerifyRange, bool, uint32_t index,
                              uint64_t min_value);

  DECLARE_EXPORT_CLASS_METHOD(VerifyBalance, bool, uint32_t index);

  DECLARE_EXPORT_CLASS_METHOD(Ecrecovery, bool, uint32_t hash_data,
                              uint32_t hash_len, uint32_t sig_data,
                              uint32_t sig_len, uint32_t id_data,
                              uint32_t id_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(Digest, bool, uint32_t in_data, uint32_t in_len,
                              uint32_t type, uint32_t out_data,
                              uint32_t out_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(VerifyRsa, bool, uint32_t pk_data,
                              uint32_t pk_len, uint32_t sig_data,
                              uint32_t sig_len, uint32_t msg_data,
                              uint32_t msg_len);

  DECLARE_EXPORT_CLASS_METHOD(VerifyRsa2, bool, uint32_t pk_data,
                              uint32_t pk_len, uint32_t sig_data,
                              uint32_t sig_len, uint32_t msg_data,
                              uint32_t msg_len, uint32_t hash_type);

  DECLARE_EXPORT_CLASS_METHOD(VerifyMessageSM2, bool, uint32_t pk_data,
                              uint32_t pk_len, uint32_t sig_data,
                              uint32_t sig_len, uint32_t msg_data,
                              uint32_t msg_len, uint32_t hash_type);

  DECLARE_EXPORT_CLASS_METHOD(Base64Encode, bool, uint32_t in_data,
                              uint32_t in_len, uint32_t out_data,
                              uint32_t out_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(Base64Decode, bool, uint32_t in_data,
                              uint32_t in_len, uint32_t out_data,
                              uint32_t out_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(abort, void);
  DECLARE_EXPORT_CLASS_METHOD(SetReturnValue, uint32_t, uint32_t in_data,
                              uint32_t in_len);

  DECLARE_EXPORT_CLASS_METHOD(GetCallResult, uint32_t, uint32_t out_data,
                              uint32_t out_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(GetCallResultSize, uint32_t);

  DECLARE_EXPORT_CLASS_METHOD(VerifyMessageECCK1, uint32_t,
                              uint32_t pubkey_data, uint32_t pubkey_len,
                              uint32_t sig_data, uint32_t sig_len,
                              uint32_t msg_data, uint32_t msg_len,
                              uint32_t hash_type);

  DECLARE_EXPORT_CLASS_METHOD(VerifyMessageECCR1, uint32_t,
                              uint32_t pubkey_data, uint32_t pubkey_len,
                              uint32_t sig_data, uint32_t sig_len,
                              uint32_t msg_data, uint32_t msg_len,
                              uint32_t hash_type);

  DECLARE_EXPORT_CLASS_METHOD(RangeProofVerify, int32_t, uint32_t ret_offset,
                              uint32_t proof_data, uint32_t proof_len,
                              uint32_t pc_list_data, uint32_t pc_list_len);

  DECLARE_EXPORT_CLASS_METHOD(AddPedersenCommit, int32_t, uint32_t ret_offset,
                              uint32_t PC_result_data, /*out*/
                              uint32_t PC_result_len_offset /*out*/,
                              uint32_t PC_left_data, /*in*/
                              uint32_t PC_left_len, uint32_t PC_right_data,
                              uint32_t PC_right_len);

  DECLARE_EXPORT_CLASS_METHOD(SubPedersenCommit, int32_t, uint32_t ret_offset,
                              uint32_t PC_result_data,
                              uint32_t PC_result_len_offset,
                              uint32_t PC_left_data, uint32_t PC_left_len,
                              uint32_t PC_right_data, uint32_t PC_right_len);

  DECLARE_EXPORT_CLASS_METHOD(CalculatePedersenCommit, int32_t,
                              uint32_t ret_offset, uint32_t dst_pc_data,
                              uint32_t dst_pc_len_offset, uint32_t src_pc_data,
                              uint32_t src_pc_len, uint32_t positive_data,
                              uint32_t positive_len, uint32_t negative_data,
                              uint32_t negative_len);

  DECLARE_EXPORT_CLASS_METHOD(PedersenCommitEqualityVerify, int32_t,
                              uint32_t ret_offset, uint32_t positive_data,
                              uint32_t positive_len, uint32_t negative_data,
                              uint32_t negative_len);
  DECLARE_EXPORT_CLASS_METHOD(CreateContract, int32_t, uint32_t src_id_data,
                              uint32_t src_id_len, uint32_t new_id_data,
                              uint32_t new_id_len);

  DECLARE_EXPORT_CLASS_METHOD(GetTransaction, uint32_t, uint32_t field,
                              uint32_t input_data, uint32_t input_len,
                              uint32_t out_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(GetRelatedTransactionListSize, uint32_t,
                              uint32_t recipient_id, uint32_t recipient_id_len,
                              uint64_t deposit_flag,
                              uint32_t count_offset /*pointer to uint64_t*/);

  DECLARE_EXPORT_CLASS_METHOD(GetRelatedTransactionList, uint32_t,
                              uint32_t recipient_id, uint32_t recipient_id_len,
                              uint64_t deposit_flag, uint64_t start_index,
                              uint32_t size, uint32_t out_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(ReadBuffer, uint32_t, uint32_t out_data_offset,
                              uint32_t out_size);

  DECLARE_EXPORT_CLASS_METHOD(ReadBufferRef, uint32_t, uint32_t out_data_offset,
                              uint32_t out_size);

  DECLARE_EXPORT_CLASS_METHOD(LiftedElgamalContractHomomorphicAdd, int32_t,
                              uint32_t first_data, uint32_t first_len,
                              uint32_t second_data, uint32_t second_len,
                              uint32_t out_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(LiftedElgamalContractHomomorphicSub, int32_t,
                              uint32_t first_data, uint32_t first_len,
                              uint32_t second_data, uint32_t second_len,
                              uint32_t out_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(LiftedElgamalScalarMutiply, int32_t,
                              uint32_t src_data, uint32_t src_len,
                              uint64_t scalar, uint32_t out_len_offset);

  DECLARE_EXPORT_CLASS_METHOD(LiftedElgamalContractZeroCheckVerify, int32_t,
                              uint32_t ciphertext_data, uint32_t ciphertext_len,
                              uint32_t proof_data, uint32_t proof_len,
                              uint32_t verify_result_offset);

  DECLARE_EXPORT_CLASS_METHOD(LiftedElgamalContractRangeVerify, int32_t,
                              uint32_t ciphertext, uint32_t ciphertext_len,
                              uint32_t per_value_bit_size, uint32_t public_key,
                              uint32_t public_key_len, uint32_t proof,
                              uint32_t proof_len,
                              uint32_t verify_result_offset);

protected:
  /* internal shared method by "CallContract" and "DelegateCall" */
  DECLARE_EXPORT_CLASS_METHOD(ExecuteMethod, int32_t,
                              std::shared_ptr<VMContext> &vm_context /*in*/,
                              const h256 &hash_id /*in*/, uint64_t value /*in*/,
                              uint32_t method_data /*in*/,
                              uint32_t method_len /*in*/,
                              uint32_t args_data /*in*/,
                              uint32_t args_len /*in*/);

  VMContextPtr vm_context_ptr_;
  std::string execute_method_;
  std::string method_params_;
  ::bytes child_output_;
  ExtVMPtr ext_vm_ptr_;

  bytesConstRef buffer_ref_;
  std::string buffer_;
};

} // namespace plugin
#endif // X_PLUGINS_MYCHAIN_INCLUDE_PLUGINS_VM_PLUGIN_WASM_EXT_WASM_H_
