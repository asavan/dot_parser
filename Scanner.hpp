#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "Tokens.hpp"
#include <fstream>
#include <map>
#include <string>

namespace ACT {
namespace DOT {

class Scanner {
  public:
    Scanner(std::string& filename);
    ~Scanner();

    Token next_token();
    Token cur_token() { return cur_; }
    std::string cur_value() { return cur_val_; }

  private:
    typedef std::map<std::string, Token> KeywordMap;

    void init();

    KeywordMap keywords_;
    Token cur_;
    std::string cur_val_;
    std::string filename_;
    std::ifstream is_;
};

} // namespace DOT
} // namespace ACT

#endif
