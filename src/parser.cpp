#include <vector>
#include "ast.hpp"
#include "token.hpp"
#include "errors.hpp"
#include <optional>
#include <iostream>
#include "parser.hpp"
#include <optional>

std::optional<AST_Op> operator_from_token(Token &tk) {
  if (operators.count(tk.type) != 0) {
    return operators[tk.type];
  }
  return std::nullopt;
}

void Parser::parse() {
  while (true) {
    Token &current = this->current();
    std::cout << "cycle start " << current.lexeme << std::endl;
    if (current.type == Token::Type::NEWLINE) {
      this->pos++;
      continue;
    } else if (current.type == Token::Type::END_OF_FILE) {
      return;
    }
    AST_Node *new_node = this->expression();
    if (new_node == nullptr) {
      std::cerr << "Got nullptr for .." << this->current().lexeme << std::endl;
      this->pos++;
      continue;
    }
    this->tree.allocate(new_node);
    this->pos++;

  }
}

// ---------------------------------------------------------------------
// HELPER METHODS
// ---------------------------------------------------------------------
Parser::Parser(std::vector<Token> stream, Reporter *reporter)
    :  stream(stream), pos(0), tree(Parse_Tree()), reporter(reporter) {}

Token &Parser::peek() {
  if (this->pos + 1 >= this->stream.size()) {
    return this->stream.back(); // will return EOF
  }
  return this->stream[this->pos + 1];
}

Token &Parser::next() {
  if (this->pos >= this->stream.size()) {
    return this->stream.back(); // will return EOF
  }
  return this->stream[this->pos++];
}

Token &Parser::current() {
  if (this->pos >= this->stream.size()) {
    return this->stream.back(); // will return EOF
  }
  return this->stream[this->pos];
}

bool Parser::peek_consume_if(Token::Type assert_type) {
  Token &tk = this->peek();
  if (tk.type == assert_type) {
    this->pos++;
    return true;
  }
  return false;
}

bool Parser::peek_consume_if(std::vector<Token::Type> assert_types) {
  Token &tk = this->peek();
  
  for (Token::Type assert_type : assert_types) {
    if (tk.type == assert_type) {
      this->pos++;
      return true;
    }
  }
  return false;
}

void Parser::error_here(Error::Type error_type, Error::Flag flag, AST_Node *expr, std::string message) {
  int line = expr->line;
  int start = expr->start;
  int stop = expr->stop;
  this->reporter->new_error(error_type,  line, start, stop, flag, message);
}

// ---------------------------------------------------------------------
// EXPRESSION PARSERS
// ---------------------------------------------------------------------

AST_Node *Parser::primary() {
  Token &tk = this->current();
  int line = tk.y;
  int start = tk.x;
  int stop = tk.x + tk.lexeme.length() - 1;

  switch (tk.type) {
  case Token::Type::NEWLINE:
    this->pos++;
    return this->primary();
  case Token::Type::SYMBOL: {
    AST_Node *n = new AST_Symbol(std::string{tk.lexeme}, line, start, stop);
    return n;
  }
  case Token::Type::STRING: {
    AST_Node *n = new AST_String(std::string(tk.lexeme), line, start, stop);
    return n;
  }
  case Token::Type::NUMBER: {
    long long int value = strtoll(std::string(tk.lexeme).c_str(), NULL, 10);
    AST_Node *n = new AST_Integer(value, line, start, stop);
    return n;
  }
  default: {
    this->reporter->new_error(Error::Type::EXPECTED_EXPRESSION, line, start, stop, Error::Flag::ABORT, "Expected a primary expression here");
    return nullptr;
  }
  }
}

// AST_Node *Parser::call() {
//   AST_Node *expr = this->primary();

//   while (true) {
//     if (this->peek_consume_if(Token::Type::LPAREN)) {
//       this->pos++;

//       // Use expression as the callee

//     }
//   }
// }

AST_Node *Parser::unary() {
  auto op = operator_from_token(this->current());
  if (op) {
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;
    AST_Unary *n = new AST_Unary(*op, line, start, stop);

    // Get the operand
    this->pos++;
    AST_Node *operand = this->unary();

    n->operand = operand;
    return n;
  }
  return this->primary();
}

AST_Node *Parser::factor() {
  AST_Node *expr = this->unary();

  if (this->peek_consume_if(std::vector{Token::Type::SLASH, Token::Type::STAR})) {
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;
    AST_Op op = *(operator_from_token(tk));
    AST_Binary *n = new AST_Binary(op, line, start, stop);
    
    // Get the right node
    this->pos++;
    AST_Node *right = this->factor();
    n->right = right;
    n->left = expr;
    return n;
  }
  return expr;
}

AST_Node *Parser::term() {
  AST_Node *expr = this->factor();

  if (this->peek_consume_if(std::vector{Token::Type::PLUS, Token::Type::MINUS})) {
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;
    AST_Op op = *(operator_from_token(tk));
    AST_Binary *n = new AST_Binary(op, line, start, stop);
    
    // Get the right node
    this->pos++;
    AST_Node *right = this->factor();
    n->right = right;
    n->left = expr;
    return n;
  }
  return expr;
}

AST_Node *Parser::comparison() {
  AST_Node *expr = this->term();

  while (this->peek_consume_if(std::vector{Token::Type::LESS, Token::Type::LESS_EQUAL, Token::Type::MORE, Token::Type::MORE_EQUAL})) {
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;
    AST_Op op = *(operator_from_token(tk));
    AST_Binary *n = new AST_Binary(op, line, start, stop);

    // Get the right node
    this->pos++;
    AST_Node *right = this->term();
    n->right = right;
    n->left = expr;   
    return n;
  }
  return expr;
}

AST_Node *Parser::logical_and() {
  AST_Node *expr = this->comparison();

  while (this->peek_consume_if(Token::Type::BAR_BAR)) {
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;
    AST_Op op = *(operator_from_token(tk));
    AST_Logical *n = new AST_Logical(op, line, start, stop);

    // Get the right node
    this->pos++;
    AST_Node *right = this->comparison();
    n->right = right;
    n->left = expr;
    return n;
  }
  return expr;
}

AST_Node *Parser::logical_or() {
  AST_Node *expr = this->logical_and();

  while (this->peek_consume_if(Token::Type::BAR_BAR)) {
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;
    AST_Op op = *(operator_from_token(tk));
    AST_Logical *n = new AST_Logical(op, line, start, stop);

    // Get the right node
    this->pos++;
    AST_Node *right = this->logical_and();
    n->right = right;
    n->left = expr;
    return n;
  }
  return expr;
}

AST_Node *Parser::assignment() {
  AST_Node *expr = this->logical_or();

  if (this->peek_consume_if(std::vector{Token::Type::ARROW, Token::Type::PLUS_EQUAL, Token::Type::MINUS_EQUAL})) {
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;
    AST_Op op = *(operator_from_token(tk));
    AST_Assignment *n = new AST_Assignment(op, line, start, stop);

    // Get the value node
    AST_Node *value = this->expression();
    
    // Assert the type of assignee
    AST_Symbol *symbol = dynamic_cast<AST_Symbol*>(expr);
    if (!symbol) {
      this->error_here(Error::Type::SYNTAX_ERROR, Error::Flag::ABORT, expr, "Expected an identifier for assignment expression");
      return nullptr;
    }

    n->assignee = expr;
    n->value = value;
  }
  return expr;
}

AST_Node *Parser::expression() {
  if (this->current().type == Token::Type::NEWLINE || this->current().type == Token::Type::SEMICOLON) {
    this->pos++;
    return this->expression();
  }
  return this->assignment();
}

// ---------------------------------------------------------------------
// STATEMENT PARSERS
// ---------------------------------------------------------------------