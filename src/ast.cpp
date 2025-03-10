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
    {Token::Type::IS, AST_Op::COMP_IS},
    {Token::Type::NOT, AST_Op::COMP_NOT},
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
      {AST_Op::COMP_IS, "IS"},
      {AST_Op::COMP_NOT, "NOT"},
      {AST_Op::INITIALIZER, "="},
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
  delete this->value;
  delete this->assignee;
}

AST_String::AST_String(std::string value, int line, int start, int stop)
    : AST_Node(line, start, stop), value(value) {}

AST_Integer::AST_Integer(long long int value, int base, int line, int start,
                         int stop)
    : AST_Node(line, start, stop), value(value), base(base) {}

AST_Float::AST_Float(double value, int line, int start, int stop)
    : AST_Node(line, start, stop), value(value) {}

AST_Symbol::AST_Symbol(std::string name, int line, int start, int stop)
    : AST_Node(line, start, stop), name(name) {}

AST_Binary::AST_Binary(AST_Op op, int line, int start, int stop)
    : AST_Node(line, start, stop), op(op) {}

AST_Binary::~AST_Binary() {
  delete this->left;
  delete this->right;
}

AST_Logical::AST_Logical(AST_Op op, int line, int start, int stop)
    : AST_Node(line, start, stop), op(op) {}

AST_Logical::~AST_Logical() {
  delete this->left;
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

AST_Lookup::AST_Lookup(AST_Node *left, int line, int start, int stop)
    : AST_Node(line, start, stop), left(left) {}

AST_Lookup::~AST_Lookup() {
  delete this->left;
  delete this->right;
}

AST_Block::AST_Block(int line, int start, int stop)
    : AST_Node(line, start, stop) {}

AST_Block::~AST_Block() {
  for (AST_Node *node : this->nodes)
    delete node;
  this->nodes.clear();
}

void AST_Block::append(AST_Node *node) { this->nodes.push_back(node); }

AST_Array_Literal::AST_Array_Literal(int line, int start, int stop)
    : AST_Node(line, start, stop) {}

AST_Array_Literal::~AST_Array_Literal() {
  for (AST_Node *item : this->elems)
    delete item;
  this->elems.clear();
}

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

AST_If_Stmt::AST_If_Stmt(int line, int start, int stop)
    : AST_Node(line, start, stop) {}

AST_If_Stmt::~AST_If_Stmt() {
  delete this->condition;
  delete this->branch_if;
  if (this->branch_else)
    delete this->branch_else.value();
}

AST_Args::AST_Args(int line, int start, int stop)
    : AST_Parameter("args", line, start, stop) {}

AST_Kwargs::AST_Kwargs(int line, int start, int stop)
    : AST_Parameter("kwargs", line, start, stop) {}

AST_Return::AST_Return(int line, int start, int stop) : AST_Node(line, start, stop) {
  this->value = std::nullopt;
}

AST_Return::~AST_Return() {
  if (this->value)
    delete this->value.value();
}

// =================================================================
// AST NODE PRINT OVERRIDES
// =================================================================

void AST_Assignment::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Assignment>\n";
  this->assignee->print(indent + 2);
  std::cout << spaces << "  <Op> " << this->op << " </Op>\n";
  this->value->print(indent + 2);
  std::cout << spaces << "</Assignment>" << std::endl;
}

void AST_String::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<String> " << this->value << " </String>"
            << std::endl;
}

void AST_Integer::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  if (this->base == 10)
    std::cout << spaces << "<Integer> " << this->value << " </Integer>"
              << std::endl;
  else if (this->base == 8)
    std::cout << spaces << "<Octal> " << this->value << " </Octal>"
              << std::endl;
  else if (this->base == 2)
    std::cout << spaces << "<Binary> " << this->value << " </Binary>"
              << std::endl;
  else if (this->base == 16)
    std::cout << spaces << "<Hex> " << this->value << " </Hex>" << std::endl;
}

void AST_Float::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Float> " << this->value << " </Float>" << std::endl;
}

void AST_Symbol::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Symbol> " << this->name << " </Symbol>" << std::endl;
}

void AST_Binary::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Binary>\n";
  this->left->print(indent + 2);
  std::cout << spaces << "  <Op> " << this->op << " </Op>\n";
  this->right->print(indent + 2);
  std::cout << spaces << "</Binary>" << std::endl;
}

void AST_Logical::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Logical>\n";
  this->left->print(indent + 2);
  std::cout << spaces << "  <Op> " << this->op << " </Op>\n";
  this->right->print(indent + 2);
  std::cout << spaces << "</Logical>" << std::endl;
}

void AST_Lookup::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Lookup>\n";
  this->left->print(indent + 2);
  this->right->print(indent + 2);
  std::cout << spaces << "</Lookup>" << std::endl;
}

void AST_Unary::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Unary>\n";
  this->operand->print(indent + 2);
  std::cout << spaces << "  <Op> " << this->op << " </Op>\n";
  std::cout << spaces << "</Unary>" << std::endl;
}

void AST_Call::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Call>\n";
  this->callee->print(indent + 2);

  std::cout << spaces << "  <Args>\n";
  for (AST_Node *a : this->args) {
    a->print(indent + 4);
  }
  std::cout << spaces << "  </Args>\n";

  std::cout << spaces << "</Call>" << std::endl;
}

void AST_Function::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Function>\n";

  std::cout << spaces << "  <Params>\n";
  for (AST_Node *p : this->params) {
    p->print(indent + 4);
  }
  std::cout << spaces << "  </Params>\n";

  if (this->return_type) {
    std::cout << spaces << "  <Return Type>\n";
    this->return_type.value()->print(indent + 4);
    std::cout << spaces << "  </Return Type>\n";
  }

  std::cout << spaces << "  <Body>\n";
  this->body->print(indent + 4);
  std::cout << spaces << "  </Body>" << std::endl;
}

void AST_Grouping::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Grouping>\n";
  this->inner->print(indent + 2);
  std::cout << spaces << "</Grouping>" << std::endl;
}

void AST_Parameter::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Parameter>\n";
  std::cout << spaces << "  <Name> " << this->name << " </Name>\n";
  this->type->print(indent + 2);

  if (this->initializer) {
    std::cout << spaces << "  <Initializer>\n";
    this->initializer.value()->print(indent + 4);
    std::cout << spaces << "  </Initializer>";
  }

  std::cout << spaces << "</Parameter>" << std::endl;
}

void AST_Block::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Block>\n";

  for (AST_Node *n : this->nodes)
    n->print(indent + 2);

  std::cout << spaces << "</Block>" << std::endl;
}

void AST_Array_Literal::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Array>\n";

  for (AST_Node *i : this->elems)
    i->print(indent + 2);

  std::cout << spaces << "</Array>" << std::endl;
}

template <size_t n_cols, size_t n_rows>
void AST_Matrix_Literal<n_cols, n_rows>::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Matrix>\n";

  for (AST_Matrix_Literal::Row row : this->rows) {
    std::cout << spaces << "  <Row>\n";
    for (AST_Node *n : row)
      n->print(indent + 4);
    std::cout << spaces << "  </Row>\n";
  }

  std::cout << spaces << "</Matrix>" << std::endl;
}

void AST_Binding::print(int indent) const {
  std::string spaces = std::string(indent, ' ');

  std::cout << spaces << "<Binding>\n";
  std::cout << spaces << "  <Name> " << this->symbol << " </Name>\n";
  std::cout << spaces << "  <Mutable?> " << this->mut << " </Mutable?>\n";
  if (this->initializer) {
    std::cout << spaces << "  <Initializer>\n";
    this->initializer.value()->print(indent + 4);
    std::cout << spaces << "  </Initializer>\n";
  }
  std::cout << spaces << "</Binding>" << std::endl;
}

void AST_If_Stmt::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<If>\n";
  std::cout << spaces << "  <Condition>\n";
  this->condition->print(indent + 4);
  std::cout << spaces << "  </Condition>\n";

  std::cout << spaces << "  <True Branch>\n";
  this->branch_if->print(indent + 4);
  std::cout << spaces << "  </True Branch>\n";

  if (this->branch_else) {
    std::cout << spaces << "  <Else Branch>\n";
    this->branch_else.value()->print(indent + 4);
    std::cout << spaces << "  </Else Branch>\n";
  }
  std::cout << spaces << "</If>\n";
}

void AST_Args::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<*Args/>" << std::endl;
}

void AST_Kwargs::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<**Kwargs/>" << std::endl;
}

void AST_Return::print(int indent) const {
  std::string spaces = std::string(indent, ' ');
  std::cout << spaces << "<Return>\n";
  if (this->value)
    this->value.value()->print(indent + 2);
  std::cout << spaces << "</Return>" << std::endl;
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

void Parse_Tree::print() {
  for (AST_Node *node : this->nodes)
    node->print(0);
}