#ifndef LEXER_H
#define LEXER_H

#include "token.hpp"
#include "errors.hpp"
#include <string_view>
#include <vector>

class Lexer {
  const std::string &stream;
  size_t cursor, line;

public:
  std::vector<Token> output;
  Reporter *reporter;

  Lexer(const std::string &source, Reporter *reporter);
  void scan();

  // Deletes the pointer to Reporter and returns the tokens
  std::vector<Token> finalize();
  
private:
  // Get the next character and advance. If EOF reached, will return '\0'
  char next();

  // Exact same implementation as `Lexer::next()` but does not advance `x`
  char peek() const;

  // Peeks ahead to see if char `ch` exists, if it does, it will consume it and
  // advance `x`
  bool expect(char ch);
};

#endif
