#include "Graph.hpp"
#include <stdio.h>
#include <strstream>

bool NotPainted(ACT::DOT::Node* n) {
    // TODO smth
    return true;
}

ACT::DOT::Edge* ACT::DOT::Graph::add_edge(Node* head, Node* tail, bool is_directed) {
    Node *h = check_node(head), *t = check_node(tail);
    Edge* e = new Edge(h, t);

    h->succ().push_back(e);
    t->pred().push_back(e);
    if (!is_directed) {
        Edge* e1 = new Edge(t, h);
        h->pred().push_back(e1);
        t->succ().push_back(e1);
    }

    return e;
}

ACT::DOT::Node* ACT::DOT::Graph::add_node(const std::string& name, Node* node) {
    std::string n = name;
    if (n == "") {
        Graph* subgr = dynamic_cast<Graph*>(node);
        if (!subgr) {
            return 0;
        }
        std::ostrstream os;
        os << "__subgraph" << nsubgr_++ << "__" << std::ends;
        n = os.str();
    }

    NodeMap::iterator fi = nodes_.find(n);

    if (fi != nodes_.end()) {
        return fi->second;
    }

    nodes_.insert(NodeMap::value_type(n, node));
    return node;
}

ACT::DOT::Node* ACT::DOT::Graph::check_node(Node* node) {
    std::string name = node->name();
    NodeMap::iterator fi = nodes_.find(name);

    if (fi != nodes_.end()) {
        return fi->second;
    }

    node = new NodeImpl(name);
    nodes_.insert(NodeMap::value_type(name, node));
    return node;
}

void ACT::DOT::Graph::roots(NodeSet& r) {
    for (NodeMap::iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn) {
        Node* node = pn->second;
        if (node->pred().empty()) {
            r.insert(node);
        }
    }
    if (r.empty() && !nodes_.empty()) {
        NodeMap::iterator pn = nodes_.begin();
        r.insert(pn->second);
    }
}

void ACT::DOT::Graph::ends(NodeSet& r) {
    for (NodeMap::iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn) {
        Node* node = pn->second;
        if (node->succ().empty()) {
            r.insert(node);
        }
    }
    if (r.empty() && !nodes_.empty()) {
        NodeMap::iterator pn = nodes_.begin();
        r.insert(pn->second);
    }
}

void ACT::DOT::Graph::clear_visited() {
    for (NodeMap::iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn) {
        Node* node = pn->second;
        node->visited(false);
    }
}

void ACT::DOT::Graph::RED() {
    for (NodeMap::iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn) {
        Node* node = pn->second;
        NodeImpl* Temp = dynamic_cast<NodeImpl*>(node);
        AttrList kkk;
        if (Temp) {
            kkk = Temp->attributes();
            if (NotPainted(node))
                Temp->add_attribute("color", "red");
        } else {
            Graph* G = dynamic_cast<Graph*>(node);
            G->RED();
        }
    }
}
void ACT::DOT::Graph::List(NodeSet& r) {
    for (NodeMap::iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn) {
        Node* node = pn->second;

        Graph* G = dynamic_cast<Graph*>(node);
        if (G)
            G->List(r);
        else
            r.insert(node);
    }
}

void ACT::DOT::Graph::add_graph_attribute(const std::string& k, const std::string& v) {
    graph_attrs_[k] = v;
}

void ACT::DOT::Graph::add_node_attribute(const std::string& k, const std::string& v) {
    node_attrs_[k] = v;
}

void ACT::DOT::Graph::add_edge_attribute(const std::string& k, const std::string& v) {
    edge_attrs_[k] = v;
}

void ACT::DOT::NodeImpl::add_attribute(const std::string& k, const std::string& v) {
    attrs_[k] = v;
}

namespace ACT {
namespace DOT {

class GraphDot {
  public:
    GraphDot(std::ostream& os) : os_(os) {}
    ~GraphDot() {}
    inline void operator()(ACT::DOT::Graph* graph) {
        // os_ << "digraph" <<"\""<< graph->name() <<"\""<< "{";
        ACT::DOT::AttrList& gr_attr = graph->graph_attributes();
        for (ACT::DOT::AttrList::iterator pa = gr_attr.begin(); pa != gr_attr.end(); ++pa) {
            os_ << pa->first << " = " << "\"" << pa->second << "\"" << "; ";
        }
        ACT::DOT::AttrList& n_attr = graph->node_attributes();
        for (ACT::DOT::AttrList::iterator pa = n_attr.begin(); pa != n_attr.end(); ++pa) {
            os_ << pa->first << " = " << "\"" << pa->second << "\"" << " ";
        }
        os_ << std::endl;
        ACT::DOT::AttrList& e_attr = graph->edge_attributes();
        for (ACT::DOT::AttrList::iterator pa = e_attr.begin(); pa != e_attr.end(); ++pa) {
            os_ << pa->first << " = " << pa->second << " ";
        }
    }

    inline void operator()(ACT::DOT::Node* node) {
        os_ << "\"" << node->name() << "\"";
        ACT::DOT::NodeImpl* ni = dynamic_cast<ACT::DOT::NodeImpl*>(node);
        if (ni) {
            NodePort& port_loc = ni->port_location();
            if (port_loc.first_ != "") {
                os_ << " ->(" << port_loc.first_ << ", " << port_loc.second_ << ") ";
            }
            std::string port_ang = ni->port_angle();
            if (port_ang != "") {
                os_ << "@ " << port_ang << " ";
            }
            ACT::DOT::AttrList& attr = ni->attributes();

            int attrsize = (int)attr.size();
            if (attrsize > 0) {
                os_ << " [";

                ACT::DOT::AttrList::iterator pa = attr.begin();
                for (int i = 0; i < attrsize - 1; i++) {

                    os_ << pa->first << " = " << "\"" << pa->second << "\"" << ", ";
                    ++pa;
                }
                os_ << pa->first << " = " << "\"" << pa->second << "\"" << " ";

                os_ << "];";
            }
        }
        os_ << std::endl;
        ACT::DOT::Edges& succs = node->succ();
        for (ACT::DOT::Edges::iterator pe = succs.begin(); pe != succs.end(); ++pe) {
            ACT::DOT::Edge* e = *pe;
            ACT::DOT::Node* n = e->tail();
            os_ << "\"" << node->name() << "\"" << " -> " << "\"" << n->name() << "\"" << std::endl;
        }
        os_ << std::endl
            << "";
    }

  private:
    std::ostream& os_;
};

std::ostream& operator<<(std::ostream& os, Graph& graph) {

    DFSs<GraphDot> dot(&graph);
    dot.run(GraphDot(os));

    return os;
}

} // namespace DOT
} // namespace ACT
