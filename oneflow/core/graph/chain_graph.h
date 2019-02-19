#ifndef ONEFLOW_CORE_GRAPH_CHAIN_GRAPH_H_
#define ONEFLOW_CORE_GRAPH_CHAIN_GRAPH_H_

#include "oneflow/core/graph/graph.h"
#include <bitset>

namespace oneflow {
// 1KB
const int64_t BITSET_SIZE = 8 * 1024;

class TaskNode;

struct Chain {
  // nodes belong to this chain
  std::vector<TaskNode*> nodes;
  // ancestors of the nodes in this chain
  std::vector<std::bitset<BITSET_SIZE>> ancestors;
  // ancestors_and_this = nodes + ancestors
  std::vector<std::bitset<BITSET_SIZE>> ancestors_and_this;

  std::pair<int64_t, int64_t> stream_area_id;
};

using ChainIt = std::list<Chain>::iterator;

class ChainEdge;

class ChainNode final : public Node<ChainNode, ChainEdge> {
 public:
  OF_DISALLOW_COPY_AND_MOVE(ChainNode);
  explicit ChainNode(const std::vector<TaskNode*>& task_nodes)
      : task_nodes_(task_nodes), chain_id_(-1) {}
  virtual ~ChainNode() = default;

  std::string VisualStr() const override;
  const std::vector<TaskNode*>& TaskNodes() const { return task_nodes_; }
  int64_t chain_id() const {
    CHECK_NE(chain_id_, -1);
    return chain_id_;
  }
  void SetChainId(int64_t val) { chain_id_ = val; }

 private:
  std::vector<TaskNode*> task_nodes_;
  int64_t chain_id_;
};

class ChainEdge final : public Edge<ChainNode, ChainEdge> {
 public:
  OF_DISALLOW_COPY_AND_MOVE(ChainEdge);
  ChainEdge() = default;
  ~ChainEdge() = default;

 private:
};

class TaskGraph;

class ChainGraph final : public Graph<ChainNode, ChainEdge> {
 public:
  OF_DISALLOW_COPY_AND_MOVE(ChainGraph);
  ChainGraph() = delete;
  ~ChainGraph() = default;

  ChainGraph(const TaskGraph& task_gph);
  const char* TypeName() const override { return "ChainGraph"; }
  const std::vector<ChainNode*>& OrderdedChainNodes() const { return ordered_chain_nodes_; }

 private:
  bool HasChainEdge(ChainNode* src, ChainNode* dst) const;
  ChainNode* ChainNode4TaskNode(TaskNode* task_node) const {
    return task_node2chain_node_.at(task_node);
  }

  void GroupTaskNodesByMachineAndCollectAncestors(
      const TaskGraph& task_gph, HashMap<int64_t, std::vector<TaskNode*>>* machine2tasks,
      HashMap<TaskNode*, HashSet<TaskNode*>>* node2ancestors) const;
  void MergeTaskNodes(const HashMap<int64_t, std::vector<TaskNode*>>& machine2tasks,
                      const HashMap<TaskNode*, HashSet<TaskNode*>>& node2ancestors,
                      std::vector<std::vector<TaskNode*>>* chains) const;
  void PrioritizeUntrainableTaskNode(std::vector<TaskNode*>* task_nodes) const;
  void PartialPriorTopoForEachNode(
      const std::list<TaskNode*> starts,
      const std::function<void(TaskNode*, const std::function<void(TaskNode*)>&)>& ForEachInNode,
      const std::function<void(TaskNode*, const std::function<void(TaskNode*)>&)>& ForEachOutNode,
      const std::function<bool(TaskNode*)>& IsPrior,
      const std::function<void(TaskNode*)>& Handler) const;
  void InitChainNode(const std::vector<std::vector<TaskNode*>>& chains);
  void InitChainEdge(const std::vector<std::vector<TaskNode*>>& chains);
  void SetChainId4ChainNode();

  const TaskGraph& task_gph_;
  HashMap<TaskNode*, ChainNode*> task_node2chain_node_;
  std::vector<ChainNode*> ordered_chain_nodes_;
};

}  // namespace oneflow

#endif  // ONEFLOW_CORE_GRAPH_CHAIN_GRAPH_H_
