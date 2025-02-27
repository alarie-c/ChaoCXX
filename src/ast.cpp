#include "ast.hpp"
#include "token.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

// ######################################################################
// Expressions
// ######################################################################

AST_Expr::AST_Expr(size_t y, size_t x0, size_t x1) : y(y), x0(x0), x1(x1) {}

AST_Expr_Symbol::AST_Expr_Symbol(const std::string_view &symbol, size_t y,
                                 size_t x0, size_t x1)
    : AST_Expr(y, x0, x1), symbol(symbol) {}

void AST_Expr_Symbol::print() const {
  std::cout << "Symbol: " << this->symbol << "\n";
}

AST_Expr_Integer::AST_Expr_Integer(signed long long int value, size_t y,
                                   size_t x0, size_t x1)
    : AST_Expr(y, x0, x1), value(value) {}

void AST_Expr_Integer::print() const {
  std::cout << "Integer: " << this->value << "\n";
}

AST_Expr_String::AST_Expr_String(const std::string_view &value, size_t y,
                                 size_t x0, size_t x1)
    : AST_Expr(y, x0, x1), value(value) {}

void AST_Expr_String::print() const {
  std::cout << "String: " << this->value << "\n";
}

AST_Expr_Binary::AST_Expr_Binary(Token &op, size_t y, size_t x0, size_t x1)
    : AST_Expr(y, x0, x1), op(op) {}

AST_Expr_Binary::~AST_Expr_Binary() {
  delete this->left;
  delete this->right;
}

void AST_Expr_Binary::print() const {
  std::cout << "{ Binary\n";
  this->left->print();
  std::cout << this->op.lexeme << "\n";
  this->right->print();
  std::cout << "}\n" << std::endl;
}

AST_Expr_Unary::AST_Expr_Unary(Token &op, size_t y, size_t x0, size_t x1)
    : AST_Expr(y, x0, x1), op(op) {}

void AST_Expr_Unary::print() const {
  std::cout << "Unary: " << this->op.lexeme << " ";
  this->operand->print();
  std::cout << std::endl;
}

AST_Expr_Function::AST_Expr_Function(size_t y, size_t x0, size_t x1)
    : AST_Expr(y, x0, x1) {}

void AST_Expr_Function::print() const {
  std::cout << "{ Function\n";
  for (const auto *p : this->params)
    p->print();
  this->return_type->print();
  std::cout << "}\n" << std::endl;
}

// ######################################################################
// Statements
// ######################################################################

AST_Stmt::AST_Stmt() {}

AST_Stmt_Binding::AST_Stmt_Binding(const Token &id, bool mut)
    : id(id), mut(mut) {}

AST_Stmt_Binding::~AST_Stmt_Binding() { delete this->value; }

void AST_Stmt_Binding::print() const {
  std::cout << "{ Binding\n";
  std::cout << this->id.lexeme;
  this->value->print();
  std::cout << "mutable: " << this->mut << "\n}" << std::endl;
}

AST_Stmt_Assignment::AST_Stmt_Assignment(const Token &op) : op(op) {}
AST_Stmt_Assignment::~AST_Stmt_Assignment() {
  delete this->assignee;
  delete this->value;
}

void AST_Stmt_Assignment::print() const {
  std::cout << "{ Assignment\n";
  this->assignee->print();
  std::cout << this->op.lexeme;
  this->value->print();
  std::cout << "}\n" << std::endl;
}

AST_Stmt_Expression::AST_Stmt_Expression(AST_Expr *expr) : expr(expr) {}
AST_Stmt_Expression::~AST_Stmt_Expression() { delete this->expr; }

void AST_Stmt_Expression::print() const {
  std::cout << "Expr: ";
  this->expr->print();
  std::cout << std::endl;
}

AST_Allocator::~AST_Allocator() {
  // Iterate through and delete all the heap node pointers,
  // Stack-allocated nodes will be freed automatically
  for (AST_Expr *node : this->heap_nodes) {
    delete node;
  }
}

void AST_Allocator::allocate(AST_Expr *node) {
  // (Note) I don't like doing this selection everytime I wonder if there's a
  // better way
  if (this->stack_index < this->BUFFER_SIZE) {
    // Allocate this node on the stack since there's space
    this->stack_buffer[this->stack_index++] = node;
  } else {
    // Allocate this node on the heap
    this->heap_nodes.push_back(node);
  }
}

void AST_Allocator::print() const {
  // Print out all the stack allocated nodes since they were allocated first
  for (size_t i = 0; i < this->stack_index; i++) {
    this->stack_buffer[i]->print();
  }

  if (this->heap_nodes.empty())
    return;

  // Then print everything on the heap
  for (const AST_Expr *node : this->heap_nodes) {
    node->print();
  }
}