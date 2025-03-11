#ifndef CBC_H
#define CBC_H

// CBC stands for "Chao Bytecode"

#include "ast.hpp"
#include <vector>

enum CBC_Instruction {
  // Ends the program immediately
  QUIT = 0,

  // Create a binding to a symbol in this scope
  // e.g. `x = 0`
  STORE_CONST,

  // Create a mutable binding to a symbol in this scope
  // e.g. `mut x = 0`
  STORE_VAR,

  // Load a constant value into memory
  // e.g. `0`
  LOAD_CONST,

  // Call a function
  // e.g. `print(...)`
  CALL,
};

class CBC_Compiler {
  std::vector<AST_Node *> ast;
  std::vector<CBC_Instruction> instructions;

  // The compiler won't take a reporter pointer because these are runtime errors
  // not compiler errors

public:
  CBC_Compiler(std::vector<AST_Node *> ast);
  // ~CBC_Compiler();

  int compile();
};

#endif