#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "ast.hpp"
#include "token.hpp"
#include "errors.hpp"
#include <optional>

class Parser {
  std::vector<Token> stream;
  int pos;

public:
  Parse_Tree tree;
  Reporter *reporter;

  Parser(std::vector<Token> stream, Reporter *reporter);
  void parse();

private:
  Token &peek();
  Token &next();
  Token &current();

  bool peek_consume_if(Token::Type asserted_type);
  bool peek_consume_if(std::vector<Token::Type> asserted_types);

  void error_here(Error::Type error_type, Error::Flag flag, AST_Node *expr, std::string message);

private:
  AST_Node *primary();
  AST_Node *function();
  AST_Node *call();
  AST_Node *unary();
  AST_Node *factor();
  AST_Node *term();
  AST_Node *comparison();
  AST_Node *logical_and();
  AST_Node *logical_or();
  AST_Node *assignment();
  AST_Node *expression(); // top-level

private:
  AST_Node *binding();
  AST_Node *statement(); // top-level
};

#endif