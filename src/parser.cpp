#include "parser.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include "token.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

Parser::Parser(std::vector<Token> &stream, Reporter *reporter)
    : stream(stream), reporter(reporter) {
  this->allocator = AST_Allocator();
}