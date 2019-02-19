#ifndef ONEFLOW_CORE_OPERATOR_CONSTANT_OP_H_
#define ONEFLOW_CORE_OPERATOR_CONSTANT_OP_H_

#include "oneflow/core/operator/operator.h"

namespace oneflow {

class ConstantOp final : public Operator {
 public:
  OF_DISALLOW_COPY_AND_MOVE(ConstantOp);
  ConstantOp() = default;
  ~ConstantOp() override = default;

  void InitFromOpConf() override;
  const PbMessage& GetCustomizedConf() const override;
  void InferBlobDescs(std::function<BlobDesc*(const std::string&)> GetBlobDesc4BnInOp,
                      const ParallelContext* parallel_ctx,
                      int64_t record_piece_size) const override;
  bool NeedInBlobWhenBackward() const override { return false; }
  bool NeedOutBlobWhenBackward() const override { return false; }
  bool IsAllOutputConst() const override { return true; }

 private:
  bool IsInputBlobAllowedModelSplit(const std::string& ibn) const override { return false; }

  void VirtualGenKernelConf(std::function<const BlobDesc*(const std::string&)> GetBlobDesc4BnInOp,
                            const ParallelContext* parallel_ctx,
                            KernelConf* kernel_conf) const override;
};

}  // namespace oneflow

#endif  // ONEFLOW_CORE_OPERATOR_CONSTANT_OP_H_
