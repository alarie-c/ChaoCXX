#ifndef PARSER_H
#define PARSE_H

#include "token.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <optional>

class Parser {
  std::vector<Token> &stream;
  int cursor = 0;
  
public:
  AST_Allocator allocator;
  Reporter *reporter;

  Parser(std::vector<Token> &stream, Reporter *reporter);
  void parse_all();

private:
  std::optional<const Token&> peek() const;
  const Token &next();

  bool expect(Token::Type type);
  bool expect(std::vector<Token::Type> types);

  void advance_until_end_stmt();
  void throw_error(Error::Type type, const AST_Node &node, std::string message);
};

#endif