#include "ast.hpp"
#include "token.hpp"
#include <iostream>
#include <string>
#include <vector>

// =================================================================
// AST OPERATORS
// =================================================================

std::map<Token::Type, AST_Op> operators = {
    {Token::Type::STAR_STAR, AST_Op::EXPONENT},
    {Token::Type::STAR, AST_Op::MULTIPLY},
    {Token::Type::SLASH, AST_Op::DIVIDE},
    {Token::Type::MODULO, AST_Op::MODULUS},
    {Token::Type::PLUS, AST_Op::ADD},
    {Token::Type::MINUS, AST_Op::SUBTRACT},
    {Token::Type::ARROW, AST_Op::ASSIGN},
    {Token::Type::STAR_EQUAL, AST_Op::ASSIGN_MULTIPLY},
    {Token::Type::SLASH_EQUAL, AST_Op::ASSIGN_DIVIDE},
    {Token::Type::PLUS_EQUAL, AST_Op::ASSIGN_INCREMENT},
    {Token::Type::MINUS_EQUAL, AST_Op::ASSIGN_DECREMENT},
    {Token::Type::PLUS_PLUS, AST_Op::INCREMENT},
    {Token::Type::MINUS_MINUS, AST_Op::DECREMENT},
    {Token::Type::BAR_BAR, AST_Op::LOGICAL_OR},
    {Token::Type::AMP_AMP, AST_Op::LOGICAL_AND},
    {Token::Type::BAR, AST_Op::BITWISE_OR},
    {Token::Type::AMP, AST_Op::BITWISE_AND},
    {Token::Type::EQUAL_EQUAL, AST_Op::COMP_EQUAL},
    {Token::Type::BANG_EQUAL, AST_Op::COMP_NOT_EQUAL},
    {Token::Type::LESS, AST_Op::COMP_LESS},
    {Token::Type::LESS_EQUAL, AST_Op::COMP_LESS_EQUAL},
    {Token::Type::MORE, AST_Op::COMP_MORE},
    {Token::Type::MORE_EQUAL, AST_Op::COMP_MORE_EQUAL},

};

std::ostream &operator<<(std::ostream &os, const AST_Op &ast_op) {
  static std::map<AST_Op, std::string> op_string = {
      {AST_Op::EXPONENT, "**"},
      {AST_Op::MULTIPLY, "*"},
      {AST_Op::DIVIDE, "/"},
      {AST_Op::MODULUS, "%"},
      {AST_Op::ADD, "+"},
      {AST_Op::SUBTRACT, "-"},
      {AST_Op::ASSIGN, "->"},
      {AST_Op::ASSIGN_MULTIPLY, "*="},
      {AST_Op::ASSIGN_DIVIDE, "/="},
      {AST_Op::ASSIGN_INCREMENT, "+="},
      {AST_Op::ASSIGN_DECREMENT, "-="},
      {AST_Op::INCREMENT, "++"},
      {AST_Op::DECREMENT, "--"},
      {AST_Op::LOGICAL_OR, "||"},
      {AST_Op::LOGICAL_AND, "&&"},
      {AST_Op::BITWISE_OR, "|"},
      {AST_Op::BITWISE_AND, "&"},
      {AST_Op::COMP_EQUAL, "=="},
      {AST_Op::COMP_NOT_EQUAL, "!="},
      {AST_Op::COMP_LESS, "<"},
      {AST_Op::COMP_LESS_EQUAL, "<="},
      {AST_Op::COMP_MORE, ">"},
      {AST_Op::COMP_MORE_EQUAL, ">="},
  };
  os << op_string[ast_op];
  return os;
}

// =================================================================
// AST NODE CONSTRUCTORS
// =================================================================

AST_Node::AST_Node(int line, int start, int stop)
    : line(line), start(start), stop(stop) {}

AST_Assignment::AST_Assignment(AST_Op op, int line, int start, int stop)
    : AST_Node(line, start, stop), op(op) {}

AST_Assignment::~AST_Assignment() {
  if (this->value)
    delete this->value;
  if (this->assignee)
    delete this->assignee;
}

AST_String::AST_String(std::string value, int line, int start, int stop)
    : AST_Node(line, start, stop), value(value) {}

AST_Integer::AST_Integer(long long int value, int line, int start, int stop)
    : AST_Node(line, start, stop), value(value) {}

AST_Symbol::AST_Symbol(std::string name, int line, int start, int stop)
    : AST_Node(line, start, stop), name(name) {}

AST_Binary::AST_Binary(AST_Op op, int line, int start, int stop)
    : AST_Node(line, start, stop), op(op) {}

AST_Binary::~AST_Binary() {
  if (this->left)
    delete this->left;
  if (this->right)
    delete this->right;
}

AST_Logical::AST_Logical(AST_Op op, int line, int start, int stop)
    : AST_Node(line, start, stop), op(op) {}

AST_Logical::~AST_Logical() {
  if (this->left)
    delete this->left;
  if (this->right)
    delete this->right;
}

AST_Unary::AST_Unary(AST_Op op, int line, int start, int stop)
    : AST_Node(line, start, stop), op(op) {}

AST_Unary::~AST_Unary() { delete this->operand; }

AST_Call::AST_Call(AST_Node *callee, int line, int start, int stop)
    : AST_Node(line, start, stop), callee(callee) {}

AST_Call::~AST_Call() {
  delete this->callee;
  for (AST_Node *node : this->args)
    delete node;
}

AST_Function::AST_Function(int line, int start, int stop)
    : AST_Node(line, start, stop) {}

AST_Function::~AST_Function() {
  if (this->return_type)
    delete this->return_type.value();

  for (AST_Node *node : this->params)
    delete node;
  this->params.clear();

  delete this->body;
}

AST_Parameter::AST_Parameter(std::string name, int line, int start, int stop)
    : AST_Node(line, start, stop), name(name) {}

AST_Parameter::~AST_Parameter() {
  if (this->initializer)
    delete this->initializer.value();
  delete this->type;
}

AST_Block::AST_Block(int line, int start, int stop)
    : AST_Node(line, start, stop) {}

AST_Block::~AST_Block() {
  for (AST_Node *node : this->nodes)
    delete node;
  this->nodes.clear();
}

void AST_Block::append(AST_Node *node) { this->nodes.push_back(node); }

AST_Binding::AST_Binding(bool mut, std::string symbol, int line, int start,
                         int stop)
    : AST_Node(line, start, stop), mut(mut), symbol(symbol) {}

AST_Binding::~AST_Binding() {
  if (this->initializer)
    delete this->initializer.value();
}

AST_Grouping::AST_Grouping(AST_Node *inner, int line, int start, int stop)
    : AST_Node(line, start, stop), inner(inner) {}

AST_Grouping::~AST_Grouping() { delete this->inner; }

// =================================================================
// AST NODE PRINT OVERRIDES
// =================================================================

void AST_Assignment::print() const {
  std::cout << "{ Assignment\n";
  this->assignee->print();
  std::cout << this->op << "\n";
  this->value->print();
  std::cout << "\n}" << std::endl;
}

void AST_String::print() const {
  std::cout << "String: " << this->value << std::endl;
}

void AST_Integer::print() const {
  std::cout << "Integer: " << this->value << std::endl;
}

void AST_Symbol::print() const {
  std::cout << "Symbol: " << this->name << std::endl;
}

void AST_Binary::print() const {
  std::cout << "{ Binary\n";
  this->left->print();
  std::cout << this->op << "\n";
  this->right->print();
  std::cout << "}" << std::endl;
}

void AST_Logical::print() const {
  std::cout << "{ Logical\n";
  this->left->print();
  std::cout << this->op << "\n";
  this->right->print();
  std::cout << "}" << std::endl;
}

void AST_Unary::print() const {
  std::cout << "{ Unary\n";
  std::cout << this->op << "\n";
  this->operand->print();
  std::cout << "}" << std::endl;
}

void AST_Call::print() const {
  std::cout << "{ Call\n";
  this->callee->print();
  for (const AST_Node *node : this->args)
    node->print();
  std::cout << "}" << std::endl;
}

void AST_Function::print() const {
  std::cout << "{ Function\n";
  for (const AST_Node *node : this->params)
    node->print();
  if (this->return_type)
    this->return_type.value()->print();
  this->body->print();
  std::cout << "}" << std::endl;
}

void AST_Grouping::print() const {
  std::cout << "{ Grouping\n";
  this->inner->print();
  std::cout << "}" << std::endl;
}

void AST_Parameter::print() const {
  std::cout << "{ Parameter\n" << this->name;
  this->type->print();
  if (this->initializer)
    this->initializer.value()->print();
  std::cout << "}" << std::endl;
}

void AST_Block::print() const {
  std::cout << "{ Block\n";
  for (const AST_Node *node : this->nodes)
    node->print();
  std::cout << "}" << std::endl;
}

void AST_Binding::print() const {
  std::cout << "{ Variable Decl\n";
  std::cout << this->symbol << "\n" << this->mut << "\n";
  if (this->initializer)
    this->initializer.value()->print();
}

// =================================================================
// PARSE TREE
// =================================================================

Parse_Tree::Parse_Tree() {}

Parse_Tree::~Parse_Tree() {
  for (AST_Node *n : this->nodes)
    delete n;
}

void Parse_Tree::allocate(AST_Node *node) { this->nodes.push_back(node); }

void Parse_Tree::print_all() {
  for (AST_Node *node : this->nodes)
    node->print();
}