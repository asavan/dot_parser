#ifndef GRAPH_FACTORY_HPP
#define GRAPH_FACTORY_HPP

#include "Graph.hpp"
#include "Tree.hpp"

namespace ACT {
namespace DOT {

class GraphFactory {
  public:
    GraphFactory(TreeNode* tree) : tree_(tree) {}
    ~GraphFactory() {}

    Graph* create();

  private:
    Node* process_node(TreeNode* node, Graph*& graph);
    void graph_attrs(TreeNode* node, Graph* graph);
    void node_attrs(TreeNode* node, Graph* graph);
    void edge_attrs(TreeNode* node, Graph* graph);
    void node_attrs(TreeNode* node, NodeImpl* gr_node);
    void port_angle(TreeNode* node, NodeImpl* gr_node);
    void port_location(TreeNode* node, NodeImpl* gr_node);
    void edge_attrs(TreeNode* node, AttrList& attrs);
    AttrList edge_attrs_;
    TreeNode* tree_;
};
} // namespace DOT
} // namespace ACT

#endif