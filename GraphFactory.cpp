#include "GraphFactory.hpp"

ACT::DOT::Graph* ACT::DOT::GraphFactory::create() {
    Graph* gr = 0;

    process_node(tree_, gr);

    return gr;
}

ACT::DOT::Node* ACT::DOT::GraphFactory::process_node(TreeNode* node, Graph*& graph) {
    bool is_strict = false;

    if (node->type() == tkStrict) {

        if (node->children().size() != 1) {
            return 0;
        }

        is_strict = true;
        node = node->children()[0];
    }

    switch (node->type()) {
    case tkDigraph: {
        graph = new Graph(node->name());
        graph->directed(true);
        graph->strict(is_strict);
        for (TreeNode::Children::const_iterator pch = node->children().begin(); pch != node->children().end(); ++pch) {
            process_node(*pch, graph);
        }
        return graph;
    } break;
    case tkGraph: {
        if (!graph) {
            graph = new Graph(node->name());
            graph->directed(false);
            graph->strict(is_strict);
            for (TreeNode::Children::const_iterator pch = node->children().begin(); pch != node->children().end(); ++pch) {
                edge_attrs_.clear();
                process_node(*pch, graph);
            }
        } else {
            for (TreeNode::Children::const_iterator pch = node->children().begin(); pch != node->children().end(); ++pch) {
                graph_attrs(*pch, graph);
            }
        }
        return graph;
    } break;
    case tkSubgraph: {
        Graph* subgr = new Graph(node->name());
        subgr->directed(graph->directed());
        graph->add_node(subgr->name(), subgr);
        for (TreeNode::Children::const_iterator pch = node->children().begin(); pch != node->children().end(); ++pch) {
            process_node(*pch, subgr);
        }
        return subgr;
    } break;
    case tkArrow:
    case tkMinusMinus: {
        TreeNode *left_child = node->children()[0], *right_child = node->children()[1];
        Node *nleft = process_node(left_child, graph), *nright = process_node(right_child, graph);
        Edge* edge = graph->add_edge(nleft, nright, node->type() == tkArrow);
        if (node->children().size() > 2) {
            for (std::size_t i = 2; i < node->children().size(); ++i) {
                TreeNode* child = node->children()[i];
                edge_attrs(child, edge_attrs_);
            }
        }
        if (edge_attrs_.size() > 0) {
            for (AttrList::iterator pa = edge_attrs_.begin(); pa != edge_attrs_.end(); ++pa) {
                edge->add_attribute(pa->first, pa->second);
            }
        }
        return nleft;
    } break;
    case tkId: {
        NodeImpl* n = new NodeImpl(node->name());
        Node* n0 = graph->add_node(n->name(), n);
        if (dynamic_cast<NodeImpl*>(n0)) {
            n = dynamic_cast<NodeImpl*>(n0);
        }
        for (TreeNode::Children::const_iterator pch = node->children().begin(); pch != node->children().end(); ++pch) {
            TreeNode* child = *pch;
            switch (child->type()) {
            case tkColon:
                port_location(child, n);
                break;
            case tkAtSign:
                port_angle(child, n);
                break;
            case tkLeftBracket:
                node_attrs(child, n);
                break;
            case tkArrow:
            case tkMinusMinus:
                return process_node(child, graph);
            }
        }
        return n;
    } break;
    case tkNode: {
        for (TreeNode::Children::const_iterator pch = node->children().begin(); pch != node->children().end(); ++pch) {
            node_attrs(*pch, graph);
        }
        return graph;
    } break;
    case tkEdge: {
        for (TreeNode::Children::const_iterator pch = node->children().begin(); pch != node->children().end(); ++pch) {
            edge_attrs(*pch, graph);
        }
        return graph;
    } break;
    case tkEqual: {
        if (node->children().size() != 2) {
            return 0;
        }
        TreeNode *left_child = node->children()[0], *right_child = node->children()[1];
        if (left_child->type() != tkId || right_child->type() != tkId) {
            return 0;
        }
        graph->add_graph_attribute(left_child->name(), right_child->name());
        return graph;
    } break;
    }
    return 0;
}

void ACT::DOT::GraphFactory::graph_attrs(TreeNode* node, Graph* graph) {
    if (node->type() != tkLeftBracket) {
        return;
    }
    for (TreeNode::Children::const_iterator pch = node->children().begin(); pch != node->children().end(); ++pch) {
        TreeNode* child = *pch;
        if (child->type() == tkEqual) {
            if (child->children().size() != 2) {
                continue;
            }
            TreeNode *left_child = child->children()[0], *right_child = child->children()[1];
            if (left_child->type() != tkId || right_child->type() != tkId) {
                continue;
            }
            graph->add_graph_attribute(left_child->name(), right_child->name());
        }
    }
}

void ACT::DOT::GraphFactory::node_attrs(TreeNode* node, Graph* graph) {
    if (node->type() != tkLeftBracket) {
        return;
    }
    for (TreeNode::Children::const_iterator pch = node->children().begin(); pch != node->children().end(); ++pch) {
        TreeNode* child = *pch;
        if (child->type() == tkEqual) {
            if (child->children().size() != 2) {
                continue;
            }
            TreeNode *left_child = child->children()[0], *right_child = child->children()[1];
            if (left_child->type() != tkId || right_child->type() != tkId) {
                continue;
            }
            graph->add_node_attribute(left_child->name(), right_child->name());
        }
    }
}

void ACT::DOT::GraphFactory::edge_attrs(TreeNode* node, Graph* graph) {
    if (node->type() != tkLeftBracket) {
        return;
    }
    for (TreeNode::Children::const_iterator pch = node->children().begin(); pch != node->children().end(); ++pch) {
        TreeNode* child = *pch;
        if (child->type() == tkEqual) {
            if (child->children().size() != 2) {
                continue;
            }
            TreeNode *left_child = child->children()[0], *right_child = child->children()[1];
            if (left_child->type() != tkId || right_child->type() != tkId) {
                continue;
            }
            graph->add_edge_attribute(left_child->name(), right_child->name());
        }
    }
}

void ACT::DOT::GraphFactory::node_attrs(TreeNode* node, NodeImpl* gr_node) {
    if (node->type() != tkLeftBracket) {
        return;
    }
    for (TreeNode::Children::const_iterator pch = node->children().begin(); pch != node->children().end(); ++pch) {
        TreeNode* child = *pch;
        if (child->type() == tkEqual) {
            if (child->children().size() != 2) {
                continue;
            }
            TreeNode *left_child = child->children()[0], *right_child = child->children()[1];
            if (left_child->type() != tkId || right_child->type() != tkId) {
                continue;
            }
            gr_node->add_attribute(left_child->name(), right_child->name());
        }
    }
}

void ACT::DOT::GraphFactory::port_angle(TreeNode* node, NodeImpl* gr_node) {
    if (node->type() != tkAtSign || node->children().size() != 1 || node->children()[0]->type() != tkId) {
        return;
    }
    gr_node->add_port_angle(node->children()[0]->name());
}

void ACT::DOT::GraphFactory::port_location(TreeNode* node, NodeImpl* gr_node) {
    if (node->type() != tkColon || node->children().size() != 1) {
        return;
    }
    node = node->children()[0];
    if (node->type() == tkId) {
        gr_node->add_port_location(node->name(), "");
    } else if (node->type() == tkComma) {
        if (node->children().size() != 2) {
            return;
        }
        TreeNode *left_child = node->children()[0], *right_child = node->children()[1];
        if (left_child->type() != tkId || right_child->type() != tkId) {
            return;
        }
        gr_node->add_port_location(left_child->name(), right_child->name());
    }
}

void ACT::DOT::GraphFactory::edge_attrs(TreeNode* node, AttrList& attrs) {
    if (node->type() != tkLeftBracket) {
        return;
    }
    for (TreeNode::Children::const_iterator pch = node->children().begin(); pch != node->children().end(); ++pch) {
        TreeNode* child = *pch;
        if (child->type() == tkEqual) {
            if (child->children().size() != 2) {
                continue;
            }
            TreeNode *left_child = child->children()[0], *right_child = child->children()[1];
            if (left_child->type() != tkId || right_child->type() != tkId) {
                continue;
            }
            attrs.insert(AttrList::value_type(left_child->name(), right_child->name()));
        }
    }
}
