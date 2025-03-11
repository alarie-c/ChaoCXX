#include "cbc.hpp"
#include "ast.hpp"
#include <vector>

template <typename T> T *cast_node(AST_Node *node) {
  return dynamic_cast<T *>(node);
}

CBC_Instruction::CBC_Instruction(CBC_Opcode code, int o1)
    : code(code), o1(o1) {}
CBC_Instruction::CBC_Instruction(CBC_Opcode code, int o1, long long int o2)
    : code(code), o1(o1), o2(o2) {}
CBC_Instruction::CBC_Instruction(CBC_Opcode code, double of)
    : code(code), of(of) {}
CBC_Instruction::CBC_Instruction(CBC_Opcode code, int o1, std::string symbol)
    : code(code), o1(o1), symbol(symbol) {}

void CBC_Instruction::print() const {
  switch (this->code) {

  case CBC_Opcode::QUIT: {
    std::cout << "QUIT, " << this->o1 << std::endl;
    break;
  }
  case CBC_Opcode::LOAD_CONST: {
    std::cout << "LOAD_CONST, " << this->o1 << ", " << this->o2 << std::endl;
    break;
  }
  case CBC_Opcode::STORE_CONST: {
    std::cout << "STORE_CONST, " << this->o1 << std::endl;
    break;
  }
  }
}

CBC_Compiler::CBC_Compiler(std::vector<AST_Node *> &ast) : ast(ast) {}

void CBC_Compiler::add(CBC_Instruction &&i) { this->program.push_back(i); }

void CBC_Compiler::load_integer(AST_Integer *node) {
  this->add(CBC_Instruction(CBC_Opcode::LOAD_CONST, 0, node->value));
};

void CBC_Compiler::binding_const(AST_Binding *node) {
  if (node->initializer) {
    this->compile_node(node->initializer.value());
  }
  this->add(CBC_Instruction(CBC_Opcode::STORE_CONST, 0, node->symbol));
}

void CBC_Compiler::compile_node(AST_Node *n) {
  switch (n->type) {

  // STORE_VAR and STORE_CONST
  case AST_Node::Type::Binding: {
    AST_Binding *binding = dynamic_cast<AST_Binding *>(n);
    if (!binding->mut) {
      this->binding_const(binding);
    }
    break;
  }

  // LOAD_CONST
  case AST_Node::Type::Integer: {
    this->load_integer(dynamic_cast<AST_Integer *>(n));
    break;
  }
  }
}

int CBC_Compiler::compile() {
  for (AST_Node *node : this->ast)
    this->compile_node(node);

  this->add(CBC_Instruction(CBC_Opcode::QUIT, 0));
  return 0;
}

void CBC_Compiler::print_program() const {
  std::cout << "\n\n[PROGRAM]:\n" << std::endl;
  for (CBC_Instruction i : this->program)
    i.print();
}