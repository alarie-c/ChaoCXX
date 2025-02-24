#include <iostream>
#include <string>
#include <memory>
#include <string_view>
#include "token.hpp"
#include "ast.hpp"

AST_Node::AST_Node(size_t y, size_t x0, size_t x1) : y(y), x0(x0), x1(x1) {}

AST_Expr_Symbol::AST_Expr_Symbol(const std::string_view &symbol, size_t y, size_t x0, size_t x1)
  : AST_Node(y, x0, x1), symbol(symbol) {}

void AST_Expr_Symbol::print() const {
  std::cout << "Symbol: " << this->symbol << "\n";
}

AST_Expr_Integer::AST_Expr_Integer(signed long long int value, size_t y, size_t x0, size_t x1)
  : AST_Node(y, x0, x1), value(value) {}

void AST_Expr_Integer::print() const {
  std::cout << "Integer: " << this->value << "\n";
}

AST_Expr_String::AST_Expr_String(const std::string_view &value, size_t y, size_t x0, size_t x1)
  : AST_Node(y, x0, x1), value(value) {}

void AST_Expr_String::print() const {
  std::cout << "String: " << this->value << "\n";
}

AST_Expr_Binary::AST_Expr_Binary(Token *op, size_t y, size_t x0, size_t x1)
  : AST_Node(y, x0, x1), op(op) {} 

AST_Expr_Binary::~AST_Expr_Binary() {
  delete this->left;
  delete this->right;
  delete this->op;
}

void AST_Expr_Binary::print() const {
  std::cout << "{ Binary\n";
  this->left->print();
  std::cout << this->op->lexeme << "\n";
  this->right->print();
  std::cout << "}\n";
}

AST_Allocator::~AST_Allocator() {
  // Iterate through and delete all the heap node pointers,
  // Stack-allocated nodes will be freed automatically
  for (AST_Node *node : this->heap_nodes) {
    delete node;
  }
}

void AST_Allocator::allocate(AST_Node *node) {
  // (Note) I don't like doing this selection everytime I wonder if there's a better way
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
  for (const AST_Node *node : this->heap_nodes) {
    node->print();
  }
}