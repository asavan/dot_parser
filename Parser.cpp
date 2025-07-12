#include <iostream>
#include "Parser.hpp"

ACT::DOT::Parser::Parser(Scanner& scanner) : scanner_(scanner)
{

}

ACT::DOT::TreeNode* ACT::DOT::Parser::parse()
{
	TreeNode* root = 0;

	try {
		graph(root);
	}
	catch(ParseError& err) {
		std::cerr << "Syntax error: " << err.error() << std::endl;
		delete root;
		return 0;
	}

	return root;
}

void ACT::DOT::Parser::graph(TreeNode*& tree)
{
	TreeNode* root = 0;
	Token tok = scanner_.next_token();

	if(tok == tkStrict) {
		tree = new TreeNode(tkStrict);
		tok = scanner_.next_token();
	}

	if(tok != tkDigraph && tok != tkGraph) {
		throw ParseError("'graph' or 'digraph' expected");
	}

	Token gr_type = tok;
	std::string gr_name;

	tok = scanner_.next_token();
	if(tok == tkId) {
		gr_name = scanner_.cur_value();
		tok = scanner_.next_token();
	}

	if(tok != tkLeftBrace) {
		throw ParseError("'{' expected");
	}

	TreeNode* gr_node = new TreeNode(gr_type, gr_name);
	if(!tree) {
		tree = gr_node;
		root = tree;
	}
	else {
		tree->add_child(gr_node);
		root = gr_node;
	}

	stmt_list(root);

	if(scanner_.cur_token() != tkRightBrace) {
		throw ParseError("'}' expected");
	}

}

void ACT::DOT::Parser::stmt_list(TreeNode*& tree)
{
	Token tok = scanner_.next_token();

	while(1) {
		if(tok == tkRightBrace) {
			break;
		}
		stmt(tree);
		if((tok = scanner_.cur_token()) == tkSemicolon) {
			tok = scanner_.next_token();
		}
	}
}

void ACT::DOT::Parser::stmt(TreeNode*& tree)
{
	Token tok = scanner_.cur_token();

	if(tok == tkGraph || tok == tkNode || tok == tkEdge) {
		TreeNode* node = new TreeNode(tok);
		tree->add_child(node);
		attr_stmt(node);
		return;
	}
	
	TreeNode* left_node = 0;

	if(tok == tkSubgraph || tok == tkLeftBrace) {
		subgraph(left_node);
	}
	else {
		if(tok != tkId) {
			throw ParseError("identifier expected");
		}

		left_node = new TreeNode(tok, scanner_.cur_value());

		tok = scanner_.next_token();
		if(tok == tkEqual) {
			TreeNode* node = new TreeNode(tok);

			tok = scanner_.next_token();
			if(tok != tkId) {
				delete left_node;
				delete node;
				throw ParseError("identifier expected");
			}
			TreeNode* right_node = new TreeNode(tok, scanner_.cur_value());
			node->add_child(left_node);
			node->add_child(right_node);
			tree->add_child(node);
			tok = scanner_.next_token();
			return;
		}


		node_id(left_node);
		tok = scanner_.cur_token();
		if(tok == tkLeftBracket) {
			tree->add_child(left_node);
			attr_list(left_node);
			return;
		}
	}
	tok = scanner_.cur_token();
	if(tok == tkArrow || tok == tkMinusMinus) {
		edge_stmt(tree, left_node);
	}
	else {
		tree->add_child(left_node);
	}
}

void ACT::DOT::Parser::attr_stmt(TreeNode*& tree)
{
	Token tok = scanner_.next_token();

	if(tok != tkLeftBracket) {
		throw ParseError("'[' expected");
	}

	attr_list(tree);
}

void ACT::DOT::Parser::attr_list(TreeNode*& tree)
{
	Token tok = scanner_.cur_token();

	while(1) {
		if(tok != tkLeftBracket) {
			return;
		}
		TreeNode* node = new TreeNode(tok);
		tree->add_child(node);
		tok = scanner_.next_token();
		a_list(node);
		if((tok = scanner_.cur_token()) != tkRightBracket) {
			throw ParseError("']' expected");
		}
		tok = scanner_.next_token();
	}
}

void ACT::DOT::Parser::a_list(TreeNode*& tree)
{
	Token tok = scanner_.cur_token();

	if(tok != tkId) {
		throw ParseError("identifier expected");
	}
	while(1) {
		TreeNode* left_node = new TreeNode(tok, scanner_.cur_value());

		tok = scanner_.next_token();
		if(tok != tkEqual) {
			delete left_node;
			throw ParseError("'=' expected");
		}

		TreeNode* node = new TreeNode(tok);

		tok = scanner_.next_token();
		if(tok != tkId) {
			delete left_node;
			delete node;
			throw ParseError("identifier expected");
		}

		TreeNode* right_node = new TreeNode(tok, scanner_.cur_value());
		node->add_child(left_node);
		node->add_child(right_node);
		tree->add_child(node);

		tok = scanner_.next_token();
		if(tok == tkComma) {
			tok = scanner_.next_token();
		}
		if(tok != tkId) {
			return;
		}
	}
}

void ACT::DOT::Parser::node_id(TreeNode*& tree)
{
	Token tok = scanner_.cur_token();

	if(tok == tkColon || tok == tkAtSign) {
		port(tree);		
	}
}

void ACT::DOT::Parser::subgraph(TreeNode*& tree)
{
	Token tok = scanner_.cur_token();
	TreeNode* node = 0;
	std::string subgr_name;

	if(tok == tkSubgraph) {
		tok = scanner_.next_token();
		if(tok != tkId) {
			throw ParseError("identifier expected");
		}
		subgr_name = scanner_.cur_value();

		tok = scanner_.next_token();
		if(tok != tkLeftBrace) {
			tree = new TreeNode(tkSubgraph, subgr_name);
			return;
		}
	}
	node = new TreeNode(tkSubgraph, subgr_name);
	stmt_list(node);
	if(scanner_.cur_token() != tkRightBrace) {
		delete node;
		throw ParseError("'}' expected");
	}
	tree = node;
	scanner_.next_token();
}

void ACT::DOT::Parser::edge_stmt(TreeNode* tree, TreeNode* left_child)
{
	Token tok = scanner_.cur_token();
	TreeNode* node = 0;

	while(1) {
		if(tok != tkArrow && tok != tkMinusMinus) {
			break;
		}
		node = new TreeNode(tok);
		node->add_child(left_child);
		tree->add_child(node);

		tok = scanner_.next_token();
		if(tok == tkId) {
			left_child = new TreeNode(tok, scanner_.cur_value());
			tok = scanner_.next_token();
			node_id(left_child);
		}
		else if(tok == tkSubgraph || tok == tkLeftBrace) {
			subgraph(left_child);
			tok = scanner_.next_token();
		}
		else {
			delete left_child;
			throw ParseError("identifier or subgraph expected");
		}
		tree = node;
	}
	node->add_child(left_child);
	if(tok == tkLeftBracket) {
		attr_list(node);
	}
}

void ACT::DOT::Parser::port(TreeNode*& tree)
{
	Token tok = scanner_.cur_token();

	if(tok == tkColon) {
		TreeNode* node = new TreeNode(tok);
		tree->add_child(node);
		port_location(node);
		tok = scanner_.cur_token();
		if(tok == tkAtSign) {
			node = new TreeNode(tok);
			tree->add_child(node);
			port_angle(node);
		}
	}
	else if(tok == tkAtSign) {
		TreeNode* node = new TreeNode(tok);
		tree->add_child(node);
		port_angle(node);
		tok = scanner_.cur_token();
		if(tok == tkColon) {
			TreeNode* node = new TreeNode(tok);
			tree->add_child(node);
			port_location(node);
		}
	}
}

void ACT::DOT::Parser::port_location(TreeNode*& tree)
{
	Token tok = scanner_.next_token();

	if(tok == tkId) {
		TreeNode* node = new TreeNode(tok, scanner_.cur_value());
		tree->add_child(node);
	}
	else if(tok == tkLeftParenthesis) {
		tok = scanner_.next_token();
		if(tok != tkId) {
			throw ParseError("identifier expected");
		}
		TreeNode* left_child = new TreeNode(tok, scanner_.cur_value());

		tok = scanner_.next_token();
		if(tok != tkComma) {
			delete left_child;
			throw ParseError("',' expected");
		}
		TreeNode* node = new TreeNode(tok);
		tok = scanner_.next_token();
		if(tok != tkId) {
			delete left_child;
			delete node;
			throw ParseError("identifier expected");
		}
		TreeNode* right_child = new TreeNode(tok, scanner_.cur_value());
		node->add_child(left_child);
		node->add_child(right_child);
		tree->add_child(node);
		tok = scanner_.next_token();
		if(tok != tkRightParenthesis) {
			throw ParseError("')' expected");
		}
	}
	else {
		throw ParseError("identifier or '(' expected");
	}
	tok = scanner_.next_token();
}

void ACT::DOT::Parser::port_angle(TreeNode*& tree)
{
	Token tok = scanner_.next_token();

	if(tok != tkId) {
		throw ParseError("identifier expected");
	}
	TreeNode* node = new TreeNode(tok, scanner_.cur_value());
	tree->add_child(node);
	tok = scanner_.next_token();
}
