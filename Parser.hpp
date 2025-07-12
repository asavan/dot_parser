#ifndef PARSER_HPP
#define PARSER_HPP

#include "Scanner.hpp"
#include "Tree.hpp"

namespace ACT {
namespace DOT {

class Parser {
  public:
    Parser(Scanner& scanner);
    ~Parser() {}

    TreeNode* parse();

  private:
    class ParseError {
      public:
        ParseError(const std::string& err) : err_(err) {}
        ~ParseError() {}
        std::string error() const { return err_; }

      private:
        std::string err_;
    };

    void graph(TreeNode*& tree);
    void stmt_list(TreeNode*& tree);
    void stmt(TreeNode*& tree);
    void attr_stmt(TreeNode*& tree);
    void attr_list(TreeNode*& tree);
    void a_list(TreeNode*& tree);
    void node_id(TreeNode*& tree);
    void edge_stmt(TreeNode* tree, TreeNode* left_child);
    void subgraph(TreeNode*& tree);
    void port(TreeNode*& tree);
    void port_location(TreeNode*& tree);
    void port_angle(TreeNode*& tree);

    Scanner& scanner_;
};

} // namespace DOT
} // namespace ACT

#endif
