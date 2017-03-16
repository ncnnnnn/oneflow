#include "graph/copy_hd_transformer_graph.h"

namespace oneflow {

void CopyHDTransfmGraph::FwBuildGraph() {
  auto copy_hd_task_node = of_dynamic_cast<const CopyHDTaskNode*>(task_node());
  // Construct Copy Operator
  OperatorConf pb_op_conf;
  pb_op_conf.set_name("");
  pb_op_conf.mutable_copy_op_conf()->set_copy_type(
      copy_hd_task_node->IsH2D() ? CopyOpConf::H2D : CopyOpConf::D2H);
  pb_op_conf.mutable_copy_op_conf()->clear_logical_blob_names();
  for (const auto& lbn : copy_hd_task_node->RelatedLbns()) {
    pb_op_conf.mutable_copy_op_conf()->add_logical_blob_names(lbn);
  }
  std::shared_ptr<const Operator> copy_op = ConstructOpFromPbConf(pb_op_conf);
  // Construct Transformer Node
  TransfmNode* copy_node = NewTransfmNode();
  copy_node->mutable_op() = copy_op;
  // Dangling Edge
  CHECK_EQ(task_node()->in_edges().size(), 1);
  for (const std::string& ibn : copy_node->op()->data_blob_name_set().input_blob_names) {
    std::string lbn = copy_node->op()->ibn2lbn(ibn);
    TransfmEdge* new_edge = NewTransfmEdge(lbn);
    new_edge->set_task_edge(of_dynamic_cast<const TaskEdge*>(*(task_node()->in_edges().begin())));
    Connect(dangling_in_edge_src(), new_edge, copy_node);
  }
  CHECK_EQ(task_node()->out_edges().size(), 1);
  for (const std::string& obn : copy_node->op()->data_blob_name_set().output_blob_names) {
    std::string lbn = copy_node->op()->obn2lbn(obn);
    TransfmEdge* new_edge = NewTransfmEdge(lbn);
    new_edge->set_task_edge(of_dynamic_cast<const TaskEdge*>(*(task_node()->out_edges().begin())));
    Connect(copy_node, new_edge, dangling_out_edge_dst());
  }
}

} // namespace oneflow
