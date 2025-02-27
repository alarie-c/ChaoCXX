#ifndef AST_H
#define AST_H

#include "token.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

struct AST_Expr {
  size_t y;
  size_t x0;
  size_t x1;

  AST_Expr(size_t y, size_t x0, size_t x1);
  virtual ~AST_Expr() = default;
  virtual void print() const = 0;
};

struct AST_Expr_Symbol : public AST_Expr {
  std::string_view symbol;

  AST_Expr_Symbol(const std::string_view &symbol, size_t y, size_t x0,
                  size_t x1);
  void print() const override;
};

struct AST_Expr_Integer : public AST_Expr {
  signed long long int value;

  AST_Expr_Integer(signed long long int value, size_t y, size_t x0, size_t x1);
  void print() const override;
};

struct AST_Expr_String : public AST_Expr {
  std::string_view value;

  AST_Expr_String(const std::string_view &value, size_t y, size_t x0,
                  size_t x1);
  void print() const override;
};

struct AST_Expr_Binary : public AST_Expr {
  AST_Expr *left;
  AST_Expr *right;
  Token &op;

  // Binary expressions are constructed with null `left` and `right` fields
  // which sould be added after they are parsed.
  // Binary expression struct is created once operator token is matched
  AST_Expr_Binary(Token &op, size_t y, size_t x0, size_t x1);
  ~AST_Expr_Binary();
  void print() const override;
};

struct AST_Expr_Unary : public AST_Expr {
  AST_Expr *operand;
  Token &op;

  AST_Expr_Unary(Token &op, size_t y, size_t x0, size_t x1);
  ~AST_Expr_Unary();
  void print() const override;
};

struct AST_Expr_Function : public AST_Expr {
  std::vector<AST_Expr *> params;
  AST_Expr *return_type;

  AST_Expr_Function(size_t y, size_t x0, size_t x1);
  ~AST_Expr_Function();
  void print() const override;
};

struct AST_Stmt {
  AST_Stmt();
  virtual ~AST_Stmt() = default;
  virtual void print() const = 0;
};

struct AST_Stmt_Binding : AST_Stmt {
  bool mut;
  const Token &id;
  AST_Expr *value;

  AST_Stmt_Binding(const Token &id, bool mut);
  ~AST_Stmt_Binding();
  void print() const override;
};

struct AST_Stmt_Assignment : AST_Stmt {
  AST_Expr *assignee;
  AST_Expr *value;
  const Token &op;

  AST_Stmt_Assignment(const Token &op);
  ~AST_Stmt_Assignment();
  void print() const override;
};

struct AST_Stmt_Expression : AST_Stmt {
  AST_Expr *expr;

  AST_Stmt_Expression(AST_Expr *expr);
  ~AST_Stmt_Expression();
  void print() const override;
};

class AST_Allocator {
  static constexpr size_t BUFFER_SIZE = 16;

  // An array on the stack to hold AST_Nodes
  AST_Expr *stack_buffer[BUFFER_SIZE];

  // An index to track where on the stack the last AST_Node is
  size_t stack_index = 0;

  // Will begin to hold AST_Nodes once the stack buffer is full
  std::vector<AST_Expr *> heap_nodes;

public:
  ~AST_Allocator();

  void allocate(AST_Expr *node);
  void print() const;
};

#endif