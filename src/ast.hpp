#ifndef AST_H
#define AST_H

#include "token.hpp"
#include <iostream>
#include <string>
#include <vector>

enum AST_Op {
  EXPONENT,
  MULTIPLY,
  DIVIDE,
  MODULUS,
  ADD,
  SUBTRACT,
  ASSIGN,
  ASSIGN_MULTIPLY,
  ASSIGN_DIVIDE,
  ASSIGN_INCREMENT,
  ASSIGN_DECREMENT,
  INCREMENT,
  DECREMENT,

  // Non-arithmetic
  LOGICAL_OR,
  LOGICAL_AND,
  BITWISE_OR,
  BITWISE_AND,

  // Comparisons
  COMP_EQUAL,
  COMP_NOT_EQUAL,
  COMP_LESS,
  COMP_LESS_EQUAL,
  COMP_MORE,
  COMP_MORE_EQUAL
};

// Utility functions for AST_Op
extern std::map<Token::Type, AST_Op> operators;
std::ostream &operator<<(std::ostream &os, const AST_Op &ast_op);

struct AST_Node {
  int line;
  int start;
  int stop;

  AST_Node(int line, int start, int stop);
  virtual ~AST_Node() = default;
  virtual void print() const = 0;
};

// Represents the assignment of one thing to a new value
// `ID | PATH` `->` `EXPR`
struct AST_Assignment : public AST_Node {
  AST_Node *assignee;
  AST_Op op;
  AST_Node *value;

  AST_Assignment(AST_Op op, int line, int start, int stop);
  ~AST_Assignment();
  void print() const override;
};

// Represents a basic string literal
// `STRING LITERAL`
struct AST_String : public AST_Node {
  std::string value;

  AST_String(std::string value, int line, int start, int stop);
  void print() const override;
};

// Represents an integer literal
// `NUMBER LITERAL` where `lexeme` contains no dot chars
struct AST_Integer : public AST_Node {
  long long int value;
  AST_Integer(long long int value, int line, int start, int stop);
  void print() const override;
};

// Represents any symbol literal
// `SYMBOL LITERAL`
struct AST_Symbol : public AST_Node {
  std::string name;
  AST_Symbol(std::string name, int line, int start, int stop);
  void print() const override;
};

// Represents a binary expression with an infix operator
// `EXPR` `+ | - | * | / | ** | %` `EXPR`
struct AST_Binary : public AST_Node {
  AST_Node *left;
  AST_Node *right;
  AST_Op op;

  // AST_Binary should have members `left` and `right` assigned after creation
  // Thusly, `left` and `right` are both `nullptr`
  AST_Binary(AST_Op op, int line, int start, int stop);
  ~AST_Binary();
  void print() const override;
};

// Represents a logical expression with an infix logical operator
// `EXPR` `|| | && | IS | IS NOT` `EXPR`
struct AST_Logical : public AST_Node {
  AST_Node *left;
  AST_Node *right;
  AST_Op op;

  // AST_Logical should have members `left` and `right` assigned after creation
  // Thusly, `left` and `right` are both `nullptr`
  AST_Logical(AST_Op op, int line, int start, int stop);
  ~AST_Logical();
  void print() const override;
};

struct AST_Unary : public AST_Node {
  AST_Node *operand;
  AST_Op op;

  AST_Unary(AST_Op op, int line, int start, int stop);
  ~AST_Unary();
  void print() const override;
};

struct AST_Call : public AST_Node {
  AST_Node *callee;
  std::vector<AST_Node *> args;

  // `args` is an empty vector upon creation
  AST_Call(AST_Node *callee, int line, int start, int stop);
  ~AST_Call();
  void print() const override;
};

// Represents a function declaration
struct AST_Function : public AST_Node {
  std::vector<AST_Node *> params;
  AST_Node *return_type;
  std::vector<AST_Node *> body;

  // Members `params`, `body`, and `return_type` are `nullptr` or empty upon
  // creation
  AST_Function(int line, int start, int stop);
  ~AST_Function();
  void print() const override;
};

// Represents any basic type of grouping expression
// `LPAR` `EXPR` `RPAR`
struct AST_Grouping : public AST_Node {
  AST_Node *inner;

  AST_Grouping(AST_Node *inner, int line, int start, int stop);
  ~AST_Grouping();
  void print() const override;
};

class Parse_Tree {
  std::vector<AST_Node *> nodes;

public:
  void allocate(AST_Node *node);
  void print_all();
  
  Parse_Tree();
  ~Parse_Tree();
};

#endif
