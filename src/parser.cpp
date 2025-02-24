#include "parser.hpp"
#include "token.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <optional>

Parser::Parser(std::vector<Token> &stream, Reporter *reporter)
  : stream(stream), reporter(reporter) {
  this->allocator = AST_Allocator();
}