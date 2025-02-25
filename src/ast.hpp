#ifndef AST_H
#define AST_H

#include "token.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

struct AST_Node {
  size_t y;
  size_t x0;
  size_t x1;

  AST_Node(size_t y, size_t x0, size_t x1);
  virtual ~AST_Node() = default;
  virtual void print() const = 0;
};

struct AST_Expr_Symbol : public AST_Node {
  std::string_view symbol;

  AST_Expr_Symbol(const std::string_view &symbol, size_t y, size_t x0,
                  size_t x1);
  void print() const override;
};

struct AST_Expr_Integer : public AST_Node {
  signed long long int value;

  AST_Expr_Integer(signed long long int value, size_t y, size_t x0, size_t x1);
  void print() const override;
};

struct AST_Expr_String : public AST_Node {
  std::string_view value;

  AST_Expr_String(const std::string_view &value, size_t y, size_t x0,
                  size_t x1);
  void print() const override;
};

struct AST_Expr_Binary : public AST_Node {
  AST_Node *left;
  AST_Node *right;
  Token *op;

  // Binary expressions are constructed with null `left` and `right` fields
  // which sould be added after they are parsed.
  // Binary expression struct is created once operator token is matched
  AST_Expr_Binary(Token *op, size_t y, size_t x0, size_t x1);
  ~AST_Expr_Binary();
  void print() const override;
};

class AST_Allocator {
  static constexpr size_t BUFFER_SIZE = 16;

  // An array on the stack to hold AST_Nodes
  AST_Node *stack_buffer[BUFFER_SIZE];

  // An index to track where on the stack the last AST_Node is
  size_t stack_index = 0;

  // Will begin to hold AST_Nodes once the stack buffer is full
  std::vector<AST_Node *> heap_nodes;

public:
  ~AST_Allocator();

  void allocate(AST_Node *node);
  void print() const;
};

#endif