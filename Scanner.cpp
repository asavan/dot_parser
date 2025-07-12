#include "Scanner.hpp"
#include <cctype>

using namespace std;

namespace ACT {
namespace DOT {

static struct tagKeywords {
    const char* keyword_;
    Token tok_;
} keywords[] = {
    {"strict", tkStrict}, {"digraph", tkDigraph}, {"graph", tkGraph}, {"node", tkNode}, {"edge", tkEdge}, {"subgraph", tkSubgraph}};

const std::size_t KEYWORDS_LEN = sizeof(keywords) / sizeof(tagKeywords);
} // namespace DOT
} // namespace ACT

void ACT::DOT::Scanner::init() {
    for (std::size_t i = 0; i < KEYWORDS_LEN; ++i) {
        keywords_.insert(KeywordMap::value_type(keywords[i].keyword_, keywords[i].tok_));
    }
}

ACT::DOT::Scanner::Scanner(std::string& filename)
    : filename_(filename), cur_(tkNone), cur_val_("") {
    init();
    is_.open(filename_.c_str());
}

ACT::DOT::Scanner::~Scanner() {
    is_.close();
}

ACT::DOT::Token ACT::DOT::Scanner::next_token() {
    Token tok = tkError;
    cur_val_ = "";

    if (!is_) {
        return (cur_ = tkNone);
    }

    char chr;
StartOver:
    is_.get(chr);

    while (isspace(chr) || chr == '\n') {
        is_.get(chr);
    }
    if (!is_) {
        return (cur_ = tkNone);
    }

    if (chr == '/') {
        is_.get(chr);
        if (!is_)
            return (cur_ = tkError);
        if (chr == '/') {

            while (is_ && chr != '\n') {
                is_.get(chr);
            }
            if (chr != '\n') {
                return (cur_ = tkError);
            }
            goto StartOver;
        } else if (chr == '*') {
            while (is_) {
                is_.get(chr);
                if (chr == '*') {
                    is_.get(chr);
                    if (!is_) {
                        return (cur_ = tkError);
                    }
                    if (chr == '/') {
                        break;
                    }
                }
            }
            if (chr != '/') {
                return (cur_ = tkError);
            }
            goto StartOver;
        } else {
            return (cur_ = tkError);
        }
    }

    switch (chr) {
    case '{':
        return (cur_ = tkLeftBrace);
    case '}':
        return (cur_ = tkRightBrace);
    case ';':
        return (cur_ = tkSemicolon);
    case '=':
        return (cur_ = tkEqual);
    case '[':
        return (cur_ = tkLeftBracket);
    case ']':
        return (cur_ = tkRightBracket);
    case ',':
        return (cur_ = tkComma);
    case ':':
        return (cur_ = tkColon);
    case '(':
        return (cur_ = tkLeftParenthesis);
    case ')':
        return (cur_ = tkRightParenthesis);
    case '@':
        return (cur_ = tkAtSign);
    case '-': {
        is_.get(chr);
        if (!is_) {
            return (cur_ = tkError);
        }

        if (chr == '>') {
            return (cur_ = tkArrow);
        } else if (chr == '-') {
            return (cur_ = tkMinusMinus);
        }
        return (cur_ = tkError);
    } break;
    default: {
        if (chr == '"') {
            is_.get(chr);
            while (is_ && chr != '"') {
                if (chr == '\\') {
                    is_.get(chr);
                }
                cur_val_ += chr;
                is_.get(chr);
            }
            if (chr != '"') {
                cur_val_ = "";
                return (cur_ = tkError);
            }
            cur_ = tok = tkId;
        } else if (isalnum(chr) || chr == '_' || chr == '.') {
            while (is_ && (isalnum(chr) || chr == '_' || chr == '.')) {
                cur_val_ += chr;
                is_.get(chr);
            }
            is_.putback(chr);
            KeywordMap::iterator fi = keywords_.find(cur_val_);
            if (fi != keywords_.end()) {
                return (cur_ = fi->second);
            }
            cur_ = tok = tkId;
        } else {
            cur_ = tok = tkError;
        }
    } break;
    }

    return tok;
}
