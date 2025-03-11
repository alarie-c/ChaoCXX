#ifndef CBC_H
#define CBC_H

// CBC stands for "Chao Bytecode"

#include "ast.hpp"
#include <vector>

enum CBC_Opcode {
  // Ends the program immediately
  // `o1`: exit code
  QUIT = 0,

  // Create a binding to a symbol in this scope
  // `o1`: register to get value from
  // `symbol`: symbol to use in table
  STORE_CONST,

  // Create a mutable binding to a symbol in this scope
  // `o1`: register to get value from
  // `symbol`: symbol to use in table
  STORE_VAR,

  // Load a constant value into memory
  // `o1`: register to store value in
  // `o2`: value to store in register
  LOAD_CONST,

  // Call a function
  // e.g. `print(...)`
  CALL,
};

struct CBC_Instruction {
  CBC_Opcode code;
  int o1 = -1;             // used for registers
  long long int o2 = -1;   // used for values
  double of = 0.0f;        // used for values
  std::string symbol = ""; // used for symbols assigned to bindings or call

  CBC_Instruction(CBC_Opcode code, int o1);
  CBC_Instruction(CBC_Opcode code, int o1, long long int o2);
  CBC_Instruction(CBC_Opcode code, double of);
  CBC_Instruction(CBC_Opcode code, int o1, std::string symbol);

  void print() const;
};

class CBC_Compiler {
  std::vector<AST_Node *> &ast;
  std::vector<CBC_Instruction> program;

  // The compiler won't take a reporter pointer because these are runtime errors
  // not compiler errors

public:
  CBC_Compiler(std::vector<AST_Node *> &ast);
  // ~CBC_Compiler();

  void print_program() const;

  void compile_node(AST_Node *n);
  int compile();

private:
  void add(CBC_Instruction &&i);

  void binding_const(AST_Binding *node);
  void load_integer(AST_Integer *node);
};

#endif