#ifndef TOKENS_HPP
#define TOKENS_HPP

namespace ACT {
namespace DOT {

enum Token {
    tkError = -1,
    tkNone = 0,
    tkId,
    tkStrict,           // strict
    tkDigraph,          // digraph
    tkGraph,            // graph
    tkNode,             // node
    tkEdge,             // edge
    tkSubgraph,         // subgraph
    tkLeftBrace,        // {
    tkRightBrace,       // }
    tkSemicolon,        // ;
    tkEqual,            // =
    tkLeftBracket,      // [
    tkRightBracket,     // ]
    tkComma,            // ,
    tkColon,            // :
    tkLeftParenthesis,  // (
    tkRightParenthesis, // )
    tkAtSign,           // @
    tkArrow,            // ->
    tkMinusMinus,       // --
    tkSlashSlash,       // //
    tkSlashStar,        // /*
    tkStarSlash         // */
};

}
} // namespace ACT

#endif
