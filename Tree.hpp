#ifndef TREE_HPP
#define TREE_HPP

#include <iostream>
#include <string>
#include <vector>

#include "Tokens.hpp"

namespace ACT {
namespace DOT {

class TreeNode {
  public:
    typedef std::vector<TreeNode*> Children;

    TreeNode(Token tok, const std::string& name = "");
    ~TreeNode();

    void add_child(TreeNode* child);

    const Children& children() const;
    Token type() const {
        return type_;
    }

    std::string name() const {
        return name_;
    }

    std::string dump_type() const;

  private:
    Token type_;
    std::string name_;
    Children children_;
};

template <typename _Process>
class PreOrder {
  public:
    PreOrder(const TreeNode& tree) : tree_(tree) {}
    ~PreOrder() {}
    void run(_Process proc_) {
        do_run(proc_, tree_, 0);
    }

  private:
    void do_run(_Process& proc, const TreeNode& node, int level) {
        proc(node, level);
        const TreeNode::Children& ch = node.children();
        for (TreeNode::Children::const_iterator pch = ch.begin(); pch != ch.end(); ++pch) {
            do_run(proc, **pch, level + 1);
        }
    }
    const TreeNode& tree_;
};

template <typename _Process>
class PostOrder {
  public:
    PostOrder(const TreeNode& tree) : tree_(tree) {}
    ~PostOrder() {}
    void run(_Process proc_) {
        do_run(proc_, tree_, 0);
    }

  private:
    void do_run(_Process& proc, const TreeNode& node, int level) {
        const TreeNode::Children& ch = node.children();
        for (TreeNode::Children::const_iterator pch = ch.begin(); pch != ch.end(); ++pch) {
            do_run(proc, **pch, level + 1);
        }
        proc(node, level);
    }
    const TreeNode& tree_;
};

class TreeDump {
  public:
    TreeDump(std::ostream& os) : os_(os) {}
    ~TreeDump() {}
    inline void operator()(const TreeNode& node, int level) {
        for (int i = 0; i < level; ++i) {
            os_ << '\t';
        }
        os_ << node.dump_type() << " " << node.name() << std::endl;
    }

  private:
    std::ostream& os_;
};

inline std::ostream& operator<<(std::ostream& os, TreeNode& node) {
    PreOrder<TreeDump> dump(node);
    dump.run(TreeDump(os));
    return os;
}
} // namespace DOT
} // namespace ACT

#endif
