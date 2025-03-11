#include "cbc.hpp"
#include "ast.hpp"
#include <vector>

CBC_Compiler::CBC_Compiler(std::vector<AST_Node *> ast) : ast(ast) {}

int CBC_Compiler::compile() {
  for (AST_Node *node : this->ast) {
  }

  return 0;
}