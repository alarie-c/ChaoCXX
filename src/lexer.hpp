#ifndef LEXER_H
#define LEXER_H

#include "token.hpp"
#include <string_view>
#include <vector>

class Lexer {
  const std::string &stream;
  size_t x, y;

public:
  std::vector<Token> output;

  Lexer(const std::string &source);
  std::vector<Token> &tokens();
  void scan();

private:
  // Get the next character and advance. If EOF reached, will return '\0'
  char next();

  // Exact same implementation as `Lexer::next()` but does not advance `x`
  char peek();

  // Peeks ahead to see if char `ch` exists, if it does, it will consume it and
  // advance `x`
  bool expect(char ch);
};

#endif
