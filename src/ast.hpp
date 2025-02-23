#ifndef AST_H
#define AST_H

#include <string_view>
#include <iostream>
#include <token.hpp>

struct Node {
  enum Type {
    NODE_EXPR_STRING,
    NODE_EXPR_SYMBOL,
    NODE_EXPR_NUMBER, // eventually expand this to different types
    NODE_EXPR_BINARY,
  };

  Type type;
  size_t y;
  size_t x0;
  size_t x1;
  union {
    ExprString string;
    ExprSymbol symbol;
    ExprNumber number;
    ExprBinary binary;
  };

  Node(Type t, size_t y, size_t x0, size_t x1);
  ~Node();
};

// Operator overload `<<` for `Node` to write to ostream
std::ostream &operator<<(std::ostream &os, const Node &node);

struct ExprString { std::string_view value = std::string_view{""}; };
struct ExprNumber { unsigned long long int value = 0; };
struct ExprSymbol { std::string_view name = std::string_view{""}; };
struct ExprBinary { Node *lhs, *rhs; Token *op; };

#endif