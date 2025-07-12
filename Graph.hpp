#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <set>
#include <map>
#include <string>
#include <stack>
#include <vector>
#include <iostream>

namespace ACT {
namespace DOT {

	typedef std::map< std::string, std::string> AttrList;

	struct NodePort {
		std::string first_, second_;
		NodePort() : first_(""), second_("") {}
	};


		class GraphDot;


	class Node;

	class Edge
	{
	public:
		Edge(Node* h, Node* t) : head_(h), tail_(t) {}
		~Edge() {}

		Node* head() { return head_; }
		Node* tail() { return tail_; }

		void add_attribute(const std::string& k, const std::string& v)
		{
			attrs_[k] = v;
		}

		AttrList& attributes() { return attrs_; }

	private:
		Node* head_, *tail_;
		AttrList attrs_;
	};

	typedef std::vector< Edge *> Edges;

	class Node
	{
	public:
		virtual Edges& pred() = 0;
		virtual Edges& succ() = 0;
		virtual std::string name() = 0;
		virtual void visited(bool v) = 0;
		virtual bool visited() = 0;
		int level;
	};

	typedef std::set<Node*> NodeSet;

	class NodeImpl : public Node
	{
	public:
		NodeImpl(const std::string& name): name_(name), visited_(false), angle_("")  {}
		~NodeImpl() {}
		virtual std::string name() { return name_; }
		virtual Edges& pred() { return pred_; }
		virtual Edges& succ() { return succ_; }
		virtual void visited(bool v) { visited_ = v; }
		virtual bool visited() { return visited_; }
		void add_attribute(const std::string& k, const std::string& v);
		AttrList& attributes() { return attrs_; }
		void add_port_angle(const std::string& a) { angle_ = a;}
		std::string port_angle() { return angle_; }
		void add_port_location(const std::string& l0, const std::string& l1) { loc_.first_ = l0; loc_.second_ = l1; }
		NodePort& port_location() { return loc_; }
	private:

		bool visited_;
		std::string name_;
		Edges pred_, succ_;
		AttrList attrs_;
		std::string angle_;
		NodePort loc_;
	};

	class Graph : public Node
	{
	public:
		Graph(const std::string& name) : name_(name), visited_(false), is_strict_(false), is_directed_(false), nsubgr_(0)  {}
		~Graph() {}

		virtual std::string name() { return name_; }
		virtual Edges& pred() { return pred_; }
		virtual Edges& succ() { return succ_; }
		virtual void visited(bool v) { visited_ = v; }
		virtual bool visited() { return visited_;}

		Node* add_node(const std::string& name, Node* node);
		Edge* add_edge(Node* head, Node* tail, bool is_directed);
		void roots(NodeSet& r);
		void ends(NodeSet& r);
		void List(NodeSet& r);
		void clear_visited();
		void RED(void);
		void PrintLevel(void);
		void strict(bool s) { is_strict_ = s; }
		bool strict() { return is_strict_; }

		void directed(bool d) { is_directed_ = d; }
		bool directed() { return is_directed_; }

		void add_graph_attribute(const std::string& k, const std::string& v);
		void add_node_attribute(const std::string& k, const std::string& v);
		void add_edge_attribute(const std::string& k, const std::string& v);

		AttrList& graph_attributes() { return graph_attrs_;}
		AttrList& node_attributes() { return node_attrs_; }
		AttrList& edge_attributes() { return edge_attrs_; }

	private:
		typedef std::map< std::string, Node* > NodeMap;

		Node* check_node(Node*);

		bool visited_;
		std::string name_;
		Edges pred_, succ_;
		NodeMap nodes_;
		bool is_strict_, is_directed_;
		AttrList graph_attrs_, node_attrs_, edge_attrs_;
		unsigned nsubgr_;
	};

	typedef std::stack< Node* > NodeStack;

	template< typename _Process >
	class DFS
	{
	public:
		DFS(Graph* graph) : graph_(graph) {}
		~DFS() {}

		void run(_Process proc)
		{
			dfs(graph_, proc);
		}

	private:
		void dfs(Graph* graph, _Process proc)
		{
			NodeStack st;
			ACT::DOT::NodeSet r;

			proc(graph);
			graph->clear_visited();
			graph->roots(r);
			for(ACT::DOT::NodeSet::iterator pn = r.begin(); pn != r.end(); ++pn) {
				ACT::DOT::Node* n = *pn;
				st.push(n);
			}
			while(!st.empty()) {
				ACT::DOT::Node* node = st.top();
				st.pop();
				node->visited(true);

				ACT::DOT::Graph* subgr = dynamic_cast<ACT::DOT::Graph*>(node);
				if(subgr) {
					dfs(subgr, proc);
				}
				else {
					proc(node);
				}
				ACT::DOT::Edges& succs = node->succ();
				for(ACT::DOT::Edges::iterator pe = succs.begin(); pe != succs.end(); ++pe) {
					ACT::DOT::Edge* e = *pe;
					ACT::DOT::Node* n = e->tail();
					if(!n->visited()) {
						st.push(n);
					}
				}

			}
		}

		Graph* graph_;
	};

	template< typename _Process >
	class DFSs
	{
	public:
		DFSs(Graph* graph) : graph_(graph) {}
		~DFSs() {}

		void run(_Process proc)
		{
			dfs(graph_, proc);
		}

	private:
		void dfs(Graph* graph, _Process proc)
		{
			NodeStack st;
			ACT::DOT::NodeSet r;

			proc(graph);
			graph->clear_visited();
			graph->List(r);
			for(ACT::DOT::NodeSet::iterator pn = r.begin(); pn != r.end(); ++pn)
			{
				ACT::DOT::Node* n = *pn;
				ACT::DOT::Graph* subgr = dynamic_cast<ACT::DOT::Graph*>(n);
				if(subgr) {
					dfs(subgr, proc);
				}
				else 
				{
					proc(n);
				}
				
			}
		}

		Graph* graph_;
	};
	class GraphDump
	{
	public:
		GraphDump(std::ostream& os) : os_(os) {}
		~GraphDump() {}

		inline void operator()(Graph* graph)
		{
			os_ << std::endl << "Graph '" << graph->name() << "' "
				<< ((graph->directed()) ? "directed " : "") << ((graph->strict()) ? "strict ": "") << ":"
				<< std::endl << "========================================" << std::endl;
			AttrList& gr_attr = graph->graph_attributes();
			os_ << "Graph attributes: ";
			for(AttrList::iterator pa = gr_attr.begin(); pa != gr_attr.end(); ++pa) {
				os_ << pa->first << " = " << pa->second << " ";
			}
			os_ << std::endl << "--------------------------------------" << std::endl;
			AttrList& n_attr = graph->node_attributes();
			os_ << "Node attributes: ";
			for(AttrList::iterator pa = n_attr.begin(); pa != n_attr.end(); ++pa) {
				os_ << pa->first << " = " << pa->second << " ";
			}
			os_ << std::endl << "--------------------------------------" << std::endl;
			AttrList& e_attr = graph->edge_attributes();
			os_ << "Edge attributes: ";
			for(AttrList::iterator pa = e_attr.begin(); pa != e_attr.end(); ++pa) {
				os_ << pa->first << " = " << pa->second << " ";
			}
			os_ << std::endl << "--------------------------------------" << std::endl;
		}

		inline void operator()(Node* node)
		{
			os_ << "Node '" << node->name() << "' ";
			NodeImpl* ni = dynamic_cast<NodeImpl*>(node);
			if(ni) {
				NodePort& port_loc = ni->port_location();
				if(port_loc.first_ != "") {
					os_ << ": (" << port_loc.first_ << ", " << port_loc.second_ << ") "; 
				}
				std::string port_ang = ni->port_angle();
				if(port_ang != "") {
					os_ << "@ " << port_ang << " ";
				}
				AttrList& attr = ni->attributes();
				for(AttrList::iterator pa = attr.begin(); pa != attr.end(); ++pa) {
					os_ << pa->first << " = " << pa->second << " ";
				}
			}
			os_ << ": ";
			ACT::DOT::Edges& succs = node->succ();
			for(ACT::DOT::Edges::iterator pe = succs.begin(); pe != succs.end(); ++pe) {
				ACT::DOT::Edge* e = *pe;
				ACT::DOT::Node* n = e->tail();
				os_ << n->name() << " ";
				AttrList& attrs = e->attributes();
				if(attrs.size() > 0) {
					os_ << "[";
					for(AttrList::iterator pa = attrs.begin(); pa != attrs.end(); ++pa) {
						os_ << pa->first << " = " << pa->second << " ";
					}
					os_ << "] ";
				}
			}
			os_ << std::endl;
		}

	private:
		std::ostream& os_;
	};

/*	inline std::ostream& operator<<(std::ostream& os, Graph& graph)
	{
		DFS<GraphDump> dump(&graph);
		dump.run(GraphDump(os));
		return os;
	}*/
std::ostream& operator<<(std::ostream& os, Graph& graph);

}}

#endif
