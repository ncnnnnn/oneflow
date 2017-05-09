#include "operator/operator.h"

namespace oneflow {
  
void Operator::InitFromProto(const OperatorProto& op_proto) {
  op_conf_ = op_proto.op_conf();
  bn_in_op2lbn_ = PbMap2HashMap(op_proto.bn_in_op2lbn());
  data_tmp_bns_ = PbVec2StdVec(op_proto.data_tmp_bns());
  input_bns_ = PbVec2StdVec(op_proto.input_bns());
  input_diff_bns_ = PbVec2StdVec(op_proto.input_diff_bns());
  output_bns_ = PbVec2StdVec(op_proto.output_bns());
  output_diff_bns_ = PbVec2StdVec(op_proto.output_diff_bns());
  model_bns_ = PbVec2StdVec(op_proto.model_bns());
  model_diff_bns_ = PbVec2StdVec(op_proto.model_diff_bns());
  model_tmp_bns_ = PbVec2StdVec(op_proto.model_tmp_bns());
}

OperatorProto Operator::ToProto() const {
  OperatorProto op_proto;
  *(op_proto.mutable_op_conf()) = op_conf_;
  *(op_proto.mutable_bn_in_op2lbn()) = HashMap2PbMap(bn_in_op2lbn_);
  *(op_proto.mutable_data_tmp_bns()) = StdVec2PbVec(data_tmp_bns_);
  *(op_proto.mutable_input_bns()) = StdVec2PbVec(input_bns_);
  *(op_proto.mutable_input_diff_bns()) = StdVec2PbVec(input_diff_bns_);
  *(op_proto.mutable_output_bns()) = StdVec2PbVec(output_bns_);
  *(op_proto.mutable_output_diff_bns()) = StdVec2PbVec(output_diff_bns_);
  *(op_proto.mutable_model_bns()) = StdVec2PbVec(model_bns_);
  *(op_proto.mutable_model_diff_bns()) = StdVec2PbVec(model_diff_bns_);
  *(op_proto.mutable_model_tmp_bns()) = StdVec2PbVec(model_tmp_bns_);
  return op_proto;
}

const std::string& Operator::Lbn4BnInOp(const std::string& bn_in_op) const {
  return bn_in_op2lbn_.at(bn_in_op);
}

void Operator::ModifyLbn4BnInOp(
    const std::string& bn_in_op,
    const std::string& lbn) {
  bn_in_op2lbn_.at(bn_in_op) = lbn;
}

const std::string& Operator::SoleIbn() const {
  CHECK_EQ(input_bns_.size(), 1);
  return *(input_bns_.begin());
}
const std::string& Operator::SoleIdbn() const {
  CHECK_EQ(input_diff_bns_.size(), 1);
  return *(input_diff_bns_.begin());
}
const std::string& Operator::SoleObn() const {
  CHECK_EQ(output_bns_.size(), 1);
  return *(output_bns_.begin());
}
const std::string& Operator::SoleOdbn() const {
  CHECK_EQ(output_diff_bns_.size(), 1);
  return *(output_diff_bns_.begin());
}
const std::string& Operator::SoleDtbn() const {
  CHECK_EQ(data_tmp_bns_.size(), 1);
  return *(data_tmp_bns_.begin());
}

void Operator::EnrollDataTmpBn(const std::string& dtbn) {
  data_tmp_bns_.push_back(dtbn);
  CHECK(bn_in_op2lbn_.emplace(dtbn, dtbn2lbn(dtbn)).second);
}

void Operator::EnrollInputBn(const std::string& ibn, bool has_diff) {
  std::string lbn = ibn2lbn(ibn);
  input_bns_.push_back(ibn);
  CHECK(bn_in_op2lbn_.emplace(ibn, lbn).second);
  if (has_diff) {
    std::string idbn = GenDiffBn(ibn);
    input_diff_bns_.push_back(idbn);
    CHECK(bn_in_op2lbn_.emplace(idbn, lbn).second);
  }
}

void Operator::EnrollOutputBn(const std::string& obn, bool has_diff) {
  std::string lbn = obn2lbn(obn);
  output_bns_.push_back(obn);
  CHECK(bn_in_op2lbn_.emplace(obn, lbn).second);
  if (has_diff) {
    std::string odbn = GenDiffBn(obn);
    output_diff_bns_.push_back(odbn);
    CHECK(bn_in_op2lbn_.emplace(odbn, lbn).second);
  }
}

void Operator::EnrollModelBn(const std::string& mbn) {
  std::string lbn = mbn2lbn(mbn);
  model_bns_.push_back(mbn);
  CHECK(bn_in_op2lbn_.emplace(mbn, lbn).second);
  std::string mdbn = GenDiffBn(mbn);
  model_diff_bns_.push_back(mdbn);
  CHECK(bn_in_op2lbn_.emplace(mdbn, lbn).second);
}

void Operator::EnrollModelTmpBn(const std::string& mtbn) {
  model_tmp_bns_.push_back(mtbn);
  CHECK(bn_in_op2lbn_.emplace(mtbn, mtbn2lbn(mtbn)).second);
}

std::string Operator::dtbn2lbn(const std::string& data_tmp_bn) const {
  return op_name() + "/" + data_tmp_bn;
}

std::string UserOperator::ibn2lbn(const std::string& input_bn) const {
  return GetStringFromSpecialConf(input_bn);
}

std::string UserOperator::obn2lbn(const std::string& output_bn) const {
  return op_name() + "/" + GetStringFromSpecialConf(output_bn);
}

std::string UserOperator::mtbn2lbn(const std::string& model_tmp_bn) const {
  return op_name() + "/" + model_tmp_bn;
}

std::string UserOperator::mbn2lbn(const std::string& model_bn) const {
  return op_name() + "/" + model_bn;
}

std::string GenDiffBn(const std::string& bn) {
  return bn + "_diff";
}

std::string GenUnDiffBn(const std::string& diff_bn) {
  CHECK_STREQ(diff_bn.substr(diff_bn.size() - 5).c_str(), "_diff");
  return diff_bn.substr(0, diff_bn.size() - 5);
}

} // namespace oneflow
