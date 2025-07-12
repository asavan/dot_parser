#include "Tree.hpp"

ACT::DOT::TreeNode::TreeNode(Token tok, const std::string& name) : type_(tok), name_(name)
{

}

ACT::DOT::TreeNode::~TreeNode()
{
	for(Children::iterator pch = children_.begin(); pch != children_.end(); ++pch) {
		delete *pch;
	}
}

void ACT::DOT::TreeNode::add_child(TreeNode* child)
{
	children_.push_back(child);
}

const ACT::DOT::TreeNode::Children& ACT::DOT::TreeNode::children() const
{
	return children_;
}

std::string ACT::DOT::TreeNode::dump_type() const
{
	switch(type_) {
	case tkArrow: return "Arrow";
	case tkAtSign: return "AtSign";
	case tkColon: return "Colon";
	case tkComma: return "Comma";
	case tkDigraph: return "Digraph";
	case tkEdge: return "Edge";
	case tkEqual: return "Equal";
	case tkError: return "Error";
	case tkGraph: return "Graph";
	case tkId: return "Id";
	case tkLeftBrace: return "LeftBrace";
	case tkLeftBracket: return "LeftBracket";
	case tkLeftParenthesis: return "LeftParenthesis";
	case tkMinusMinus: return "MinusMinus";
	case tkNode: return "Node";
	case tkNone: return "Node";
	case tkRightBrace: return "RightBrace";
	case tkRightBracket: return "RightBracket";
	case tkRightParenthesis: return "RightParenthesis";
	case tkSemicolon: return "Semicolon";
	case tkSlashSlash: return "SlashSlash";
	case tkSlashStar: return "SlashStar";
	case tkStrict: return "Strict";
	case tkSubgraph: return "Subgraph";
	}
	return "???";
}
