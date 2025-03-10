#include "parser.hpp"
#include "ast.hpp"
#include "errors.hpp"
#include "token.hpp"
#include <algorithm>
#include <array>
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

// ---------------------------------------------------------------------
// HELPER FUNCTIONS
// ---------------------------------------------------------------------

std::optional<AST_Op> operator_from_token(Token &tk) {
  if (operators.count(tk.type) != 0) {
    return operators[tk.type];
  }
  return std::nullopt;
}

AST_Node *parse_number(std::string str, int line, int start, int stop) {
  // Remove underscores
  str.erase(std::remove(str.begin(), str.end(), '_'), str.end());
  std::cout << "AFTER ERASE: " << str << std::endl;

  if (str[0] == '0') {
    // // Directly handle the '0' case
    // if (str.length() == 1) {
    //   std::cout << "Zero detected!" << std::endl;
    //   AST_Node *n = new AST_Integer(0, 10, line, start, stop);
    //   return n;
    // }

    if (str.length() > 1) {
      // Binary representation
      if (str[1] == 'B' || str[1] == 'b') {
        str.erase(0, 2);
        std::cout << "PARSING N: " << str << std::endl;
        long long int value = strtoll(str.c_str(), NULL, 2);
        AST_Node *n = new AST_Integer(value, 2, line, start, stop);
        return n;

        // Hexadecimal
      } else if (str[1] == 'X' || str[1] == 'x') {
        str.erase(0, 2);
        std::cout << "PARSING N: " << str << std::endl;
        long long int value = strtoll(str.c_str(), NULL, 16);
        AST_Node *n = new AST_Integer(value, 16, line, start, stop);
        return n;

        // Octal
      } else if (str[1] == 'O' || str[1] == 'o') {
        str.erase(0, 2);
        std::cout << "PARSING N: " << str << std::endl;
        long long int value = strtoll(str.c_str(), NULL, 8);
        AST_Node *n = new AST_Integer(value, 8, line, start, stop);
        return n;
      }
    }
  }

  if (str.find('.') != std::string::npos) {
    double value = strtod(str.c_str(), NULL);
    AST_Node *n = new AST_Float(value, line, start, stop);
    return n;
  }

  std::cout << "normal integer" << std::endl;
  long long int value = strtoll(str.c_str(), NULL, 10);
  AST_Node *n = new AST_Integer(value, 10, line, start, stop);
  return n;
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
    AST_Node *new_node = this->statement();
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
    : stream(stream), pos(0), tree(Parse_Tree()), reporter(reporter) {}

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

bool Parser::peek_consume_if_ignore_newlines(Token::Type assert_type) {
  size_t original_pos = this->pos;

  Token &tk = this->peek();
  if (tk.type == assert_type) {
    this->pos++;
    return true;
  } else if (tk.type == Token::Type::NEWLINE) {
    this->pos++;
    return this->peek_consume_if_ignore_newlines(assert_type);
  }
  // Reset the original position so that we don't consume anything unless the
  // token as found
  this->pos = original_pos;
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

void Parser::error_here(Error::Type error_type, Error::Flag flag,
                        AST_Node *expr, std::string message) {
  int line = expr->line;
  int start = expr->start;
  int stop = expr->stop;
  this->reporter->new_error(error_type, line, start, stop, flag, message);
}

template <typename T> bool Parser::assert_node_type(AST_Node *node) {
  return dynamic_cast<T *>(node) != nullptr;
}

// ---------------------------------------------------------------------
// HELPER PARSERS
// ---------------------------------------------------------------------

std::vector<AST_Parameter *> Parser::function_parameters() {
  std::vector<AST_Parameter *> params;

  if (this->current().type == Token::Type::RPAREN)
    return params;

  while (!this->peek_consume_if(Token::Type::RPAREN)) {
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;

    if (params.size() > 255) {
      this->reporter->new_error(
          Error::Type::TOO_MANY_PARAMS, line, start, stop, Error::Flag::ABORT,
          "Functions cannot have more than 255 parameters");
      this->pos++;
      continue;
    }

    int args = 0;
    switch (tk.type) {
    case Token::Type::STAR: {
      std::cout << "STAR" << std::endl;
      this->pos++;
      args = 1;
      break;
    }
    case Token::Type::STAR_STAR: {
      std::cout << "STARSTAR" << std::endl;
      this->pos++;
      args = 2;
      break;
    }
    }

    tk = this->current();
    line = tk.y;
    start = tk.x;
    stop = tk.x + tk.lexeme.length() - 1;

    if (tk.type != Token::Type::SYMBOL) {
      this->reporter->new_error(
          Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT,
          ("Expected a symbol for function parameter, got '" +
           std::string{tk.lexeme} + "' instead"));
      this->pos++;
      continue;
    }
    std::string name = std::string{tk.lexeme};

    if (args == 0) {
      if (!this->peek_consume_if(Token::Type::COLON)) {
        this->reporter->new_error(
            Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT,
            "Expected type annotation after this function parameter");
        break;
      }

      this->pos++;
      AST_Node *type = this->expression();

      // (todo) enforce type
      // (todo) also enforce that it isn't nullptr

      AST_Parameter *p = new AST_Parameter(name, line, start, stop);
      p->type = type;

      // (todo) look for initializer

      params.push_back(p);
    } else if (args == 1) {
      AST_Parameter *p = new AST_Args(line, start, stop);
      params.push_back(p);
    } else {
      AST_Parameter *p = new AST_Kwargs(line, start, stop);
      params.push_back(p);
    }

    if (this->peek_consume_if(Token::Type::COMMA)) {
      this->pos++;
      continue;
    } else {
      if (this->peek_consume_if(Token::Type::RPAREN))
        break;
      this->reporter->new_error(
          Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT,
          "Expected a ')' after this to close function parameters");
      break;
    }
  }

  return params;
}

// This parsers ends when current() = RCURL
AST_Block *Parser::block() {
  Token &tk = this->current();
  int line = tk.y;
  int start = tk.x;
  int stop = tk.x + tk.lexeme.length() - 1;
  AST_Block *node = new AST_Block(line, start, stop);

  while (true) {
    // Skip all newlines at the beginning of looking for statements
    if (this->current().type == Token::Type::NEWLINE) {
      this->pos++;
      continue;
    }

    if (this->current().type == Token::Type::RCURL)
      break;

    AST_Node *expr = this->statement();
    if (expr != nullptr)
      node->append(expr);

    std::cout << "After expression: " << this->current().lexeme << std::endl;

    while (true) {
      Token &tk = this->current();
      if (tk.type == Token::Type::NEWLINE || tk.type == Token::Type::SEMICOLON)
        break;

      // In this scenario, the user probably tried to close an expression
      // without a newline or semicolon after the last statements e.g.
      // `function() { return 5 }`
      // should be: `function() { return 5; }`
      if (tk.type == Token::Type::END_OF_FILE ||
          tk.type == Token::Type::RCURL) {
        this->reporter->new_error(
            Error::Type::SYNTAX_ERROR, tk.y, tk.x,
            tk.x + tk.lexeme.length() - 1, Error::Flag::ABORT,
            "Expected a semicolon or newline to close the previous statement");
        return node;
      }
      this->pos++;
    }

    // Next statement
    this->pos++;
  }

  return node;
}

std::vector<AST_Node *> Parser::call_arguments() {
  std::vector<AST_Node *> args;

  if (this->current().type == Token::Type::RPAREN)
    return args;

  while (this->current().type != Token::Type::RPAREN) {
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;

    if (args.size() > 255) {
      this->reporter->new_error(
          Error::Type::TOO_MANY_ARGS, line, start, stop, Error::Flag::ABORT,
          "Function calls cannot have more than 255 arguments");
      this->pos++;
      continue;
    }

    AST_Node *expr = this->expression();

    if (this->peek_consume_if(Token::Type::EQUAL)) {
      Token &tk = this->current();
      int line = tk.y;
      int start = tk.x;
      int stop = tk.x + tk.lexeme.length() - 1;
      AST_Assignment *init =
          new AST_Assignment(AST_Op::INITIALIZER, line, start, stop);

      this->pos++;

      init->assignee = expr;
      init->value = this->expression();
      args.push_back(init);
    } else
      args.push_back(expr);

    // if the next thing isnt a comma, then expect RPAREN to close
    if (this->peek_consume_if(Token::Type::COMMA)) {
      this->pos++;
      continue;
    }

    if (this->peek_consume_if(Token::Type::RPAREN))
      std::cout << "Got the RPAR here" << std::endl;
    break;

    tk = this->current();
    line = tk.y;
    start = tk.x;
    stop = tk.x + tk.lexeme.length() - 1;
    this->reporter->new_error(
        Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT,
        "Expected an ')' to close function call argument, did you forget a "
        "comma after the previous argument?");
    return args;
  }
  std::cout << "Got the RPAR here instead" << std::endl;
  return args;
}

AST_Node *Parser::array_literal(Token &tk) {
  int line = tk.y;
  int start = tk.x;
  int stop = tk.x + tk.lexeme.length() - 1;

  this->pos++; // consume [

  std::vector<AST_Node *> elems;
  while (this->current().type != Token::Type::RBRAC) {
    if (this->current().type == Token::Type::SEMICOLON) {
      // indicates the matrix case
      std::vector<std::vector<AST_Node *>> rows;
    }

    AST_Node *expr = this->expression();
    if (expr == nullptr) {
      tk = this->current();
      line = tk.y;
      start = tk.x;
      stop = tk.x + tk.lexeme.length() - 1;

      this->reporter->new_error(Error::Type::SYNTAX_ERROR, line, start, stop,
                                Error::Flag::ABORT,
                                "Invalid expression in array literal");
    } else
      elems.push_back(expr);

    if (this->peek_consume_if_ignore_newlines(Token::Type::COMMA)) {
      this->pos++;
      continue;
    } else if (this->peek_consume_if_ignore_newlines(Token::Type::RBRAC)) {
      break;
    } else {
      tk = this->current();
      line = tk.y;
      start = tk.x;
      stop = tk.x + tk.lexeme.length() - 1;

      this->reporter->new_error(
          Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT,
          "Expected a ',' to continue the array literal, or a ']' to close it");
      break;
    }
  }

  // current = RBRAC
  AST_Array_Literal *node = new AST_Array_Literal(line, start, stop);
  node->elems = elems;
  return node;
}

// ---------------------------------------------------------------------
// EXPRESSION PARSERS
// ---------------------------------------------------------------------

AST_Node *Parser::primary() {
  Token &tk = this->current();
  int line = tk.y;
  int start = tk.x;
  int stop = tk.x + tk.lexeme.length() - 1;

  std::cout << "Starting primary: " << tk.lexeme << std::endl;

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
  case Token::Type::NUMBER:
    return parse_number(std::string{tk.lexeme}, line, start, stop);
  case Token::Type::LBRAC:
    return this->array_literal(tk);
  default: {
    this->reporter->new_error(Error::Type::EXPECTED_EXPRESSION, line, start,
                              stop, Error::Flag::ABORT,
                              "Expected a primary expression here");
    return nullptr;
  }
  }
}

AST_Node *Parser::function() {
  Token &tk = this->current();
  int line = tk.y;
  int start = tk.x;
  int stop = tk.x + tk.lexeme.length() - 1;

  if (tk.type == Token::Type::FUNCTION) {
    if (!this->peek_consume_if(Token::Type::LPAREN)) {
      this->reporter->new_error(
          Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT,
          "Expected parameters or '()' after 'function' keyword");
      // (todo) figure out how to recover here ^^^
    }

    this->pos++;
    std::vector<AST_Parameter *> params = this->function_parameters();

    // look for the return type
    std::optional<AST_Node *> return_type = std::nullopt;
    if (this->peek_consume_if(Token::Type::COLON)) {
      this->pos++; // consume the :
      return_type = this->expression();
      // (todo) enforce return_type
    }

    AST_Function *node = new AST_Function(line, start, stop);

    while (this->peek_consume_if(Token::Type::NEWLINE))
      this->pos++;

    if (this->peek_consume_if(Token::Type::LCURL)) {
      this->pos++;
      AST_Block *block = this->block();
      if (block != nullptr)
        node->body = block;

    } else {
      this->reporter->new_error(Error::Type::SYNTAX_ERROR, line, start, stop,
                                Error::Flag::ABORT,
                                "Expected a '{' to open this function's block");
    }

    node->params = params;
    node->return_type = return_type;
    return node;
  }
  return this->primary();
}

AST_Node *Parser::call() {
  AST_Node *expr = this->function();

  while (true) {
    if (this->peek_consume_if(Token::Type::LPAREN)) {
      Token &tk = this->current();
      int line = tk.y;
      int start = tk.x;
      int stop = tk.x + tk.lexeme.length() - 1;

      AST_Call *node = new AST_Call(expr, line, start, stop);
      this->pos++;

      // Use expression as the callee
      std::vector<AST_Node *> args = this->call_arguments();
      if (this->current().type != Token::Type::RPAREN) {
        std::cerr << "ERROR parsing function args missing RPAREN" << std::endl;
        //return nullptr;

        this->reporter->new_error(Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT, "This function call is missing the closing ')'");

        // try returning whatever this ended up being, although it may shoot us in the foot
      }

      node->args = args;
      expr = node;
      std::cout << "After call: " << this->current().lexeme << std::endl;
    } else
      break;
  }

  return expr;
}

AST_Node *Parser::lookup() {
  AST_Node *expr = this->call();

  while (true) {
    if (this->peek_consume_if(Token::Type::LBRAC)) {
      Token &tk = this->current();
      int line = tk.y;
      int start = tk.x;
      int stop = tk.x + tk.lexeme.length() - 1;
      AST_Lookup *node = new AST_Lookup(expr, line, start, stop);

      // Get the index
      this->pos++;
      AST_Node *right = this->term();
      node->right = right;

      if (!this->peek_consume_if_ignore_newlines(Token::Type::RBRAC)) {
        Token &tk = this->current();
        int line = tk.y;
        int start = tk.x;
        int stop = tk.x + tk.lexeme.length() - 1;
        this->reporter->new_error(Error::Type::SYNTAX_ERROR, line, start, stop,
                                  Error::Flag::ABORT,
                                  "Expected a ']' to close the array lookup");
        // pretend like we consumed it anyway
      }
      std::cout << "after the lookip: " << this->current().lexeme << std::endl;
      expr = node;
    } else
      break;
  }

  return expr;
}

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
  return this->lookup();
}

AST_Node *Parser::factor() {
  AST_Node *expr = this->unary();

  if (this->peek_consume_if(std::vector{Token::Type::SLASH, Token::Type::STAR,
                                        Token::Type::MODULO})) {
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

  if (this->peek_consume_if(
          std::vector{Token::Type::PLUS, Token::Type::MINUS})) {
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

AST_Node *Parser::equality() {
  AST_Node *expr = this->term();

  while (this->peek_consume_if(
      std::vector{Token::Type::EQUAL_EQUAL, Token::Type::BANG_EQUAL,
                  Token::Type::IS, Token::Type::NOT})) {
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

AST_Node *Parser::comparison() {
  AST_Node *expr = this->equality();

  while (this->peek_consume_if(
      std::vector{Token::Type::LESS, Token::Type::LESS_EQUAL, Token::Type::MORE,
                  Token::Type::MORE_EQUAL})) {
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

  if (this->peek_consume_if(std::vector{Token::Type::ARROW,
                                        Token::Type::PLUS_EQUAL,
                                        Token::Type::MINUS_EQUAL})) {
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;

    AST_Op op = *(operator_from_token(tk));
    this->pos++;
    AST_Assignment *n = new AST_Assignment(op, line, start, stop);

    // Get the value node
    AST_Node *value = this->expression();

    // Assert the type of assignee
    AST_Symbol *symbol = dynamic_cast<AST_Symbol *>(expr);
    if (!symbol) {
      this->error_here(Error::Type::SYNTAX_ERROR, Error::Flag::ABORT, expr,
                       "Expected an identifier for assignment expression");
      return nullptr;
    }

    n->assignee = expr;
    n->value = value;
    return n;
  }
  return expr;
}

AST_Node *Parser::expression() {
  if (this->current().type == Token::Type::NEWLINE ||
      this->current().type == Token::Type::SEMICOLON) {
    this->pos++;
    return this->expression();
  }
  return this->assignment();
}

// ---------------------------------------------------------------------
// STATEMENT PARSERS
// ---------------------------------------------------------------------

void Parser::skip_to_endof_statement() {
  while (true) {
    Token &tk = this->current();
    switch (tk.type) {
    case Token::Type::NEWLINE:
    case Token::Type::SEMICOLON:
    case Token::Type::END_OF_FILE:
      return;
    default:
      this->pos++;
    }
  }
}

AST_Node *Parser::if_stmt(Token &token) {
  this->pos++; // consume IF
  int line = token.y;
  int start = token.x;
  int stop = token.x + token.lexeme.length() - 1;
  AST_If_Stmt *node = new AST_If_Stmt(line, start, stop);

  // Get the condition
  // Also, allow this to be a nullptr if neccesary, we still want to parse the
  // body
  AST_Node *condition = this->expression();
  AST_Node *branch_if;

  if (this->peek_consume_if_ignore_newlines(Token::Type::LCURL)) {
    // Expect a block for the branch body
    this->pos++;
    branch_if = this->block();
    if (branch_if == nullptr) {
      this->reporter->new_error(
          Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT,
          "The body statement for this selection statement is invalid");
    }

    // Expect the RCURL to close
    if (this->current().type != Token::Type::RCURL) {
      Token &tk = this->current();
      int line = tk.y;
      int start = tk.x;
      int stop = tk.x + tk.lexeme.length() - 1;

      // If this breaks then throw a tantrum and do absolutely nothing to fix it
      // so we can return the rest of this crap
      this->reporter->new_error(Error::Type::SYNTAX_ERROR, line, start, stop,
                                Error::Flag::ABORT,
                                "Expected a '}' to close the previous block");
    }
    // this->pos++;
    std::cout << "IF AFTER BLOCK: " << this->current().lexeme << std::endl;

  } else if (this->peek_consume_if_ignore_newlines(Token::Type::END_OF_FILE)) {
    // There is an error here
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;
    this->reporter->new_error(
        Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT,
        "There is no body statement for this selection statement");
    return nullptr;

  } else {
    // Expect just one statement to satisfy the body
    this->pos++;
    branch_if = this->statement();
    if (branch_if == nullptr) {
      this->reporter->new_error(
          Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT,
          "The body statement for this selection statement is invalid");
    }
    std::cout << "Current after the if branch: " << this->current().lexeme
              << std::endl;
  }
  std::optional<AST_Node *> branch_else = std::nullopt;

  if (this->peek_consume_if_ignore_newlines(Token::Type::ELSE)) {
    // this->pos++;
    Token &tk = this->current();
    line = tk.y;
    start = tk.x;
    stop = tk.x + tk.lexeme.length() - 1;

    if (this->peek_consume_if_ignore_newlines(Token::Type::IF)) {
      // This is for an else if block
      this->pos++; // consume the IF
      branch_else = this->if_stmt(tk);

    } else {
      // This is for the else block
      if (this->peek_consume_if_ignore_newlines(Token::Type::LCURL)) {
        // Expect a block for the branch body
        this->pos++;
        branch_else = this->block();
        if (branch_else == nullptr) {
          this->reporter->new_error(Error::Type::SYNTAX_ERROR, line, start,
                                    stop, Error::Flag::ABORT,
                                    "The body statement for the else branch in "
                                    "this selection statement is invalid");
        }

        // Expect the RCURL to close
        if (this->current().type != Token::Type::RCURL) {
          Token &tk = this->current();
          int line = tk.y;
          int start = tk.x;
          int stop = tk.x + tk.lexeme.length() - 1;
          this->reporter->new_error(
              Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT,
              "Expected a '}' to close the previous block");
        }
        this->pos++;
        std::cout << "IF AFTER BLOCK DOS: " << this->current().lexeme
                  << std::endl;

      } else if (this->peek_consume_if_ignore_newlines(
                     Token::Type::END_OF_FILE)) {
        // There is an error here
        Token &tk = this->current();
        int line = tk.y;
        int start = tk.x;
        int stop = tk.x + tk.lexeme.length() - 1;
        this->reporter->new_error(Error::Type::SYNTAX_ERROR, line, start, stop,
                                  Error::Flag::ABORT,
                                  "There is no body statement for the else "
                                  "branch in this selection statement");
        return nullptr;

      } else {
        // Expect just one statement to satisfy the body
        this->pos++;
        branch_else = this->statement();
        if (branch_else == nullptr) {
          this->reporter->new_error(Error::Type::SYNTAX_ERROR, line, start,
                                    stop, Error::Flag::ABORT,
                                    "The body statement for the else branch in "
                                    "this selection statement is invalid");
        }
      }
      this->pos++;
    }
  }

  node->condition = condition;
  node->branch_if = branch_if;
  node->branch_else = branch_else;
  return node;
}

AST_Node *Parser::initialized_binding(Token &token, bool mut) {
  this->pos++; // consume =
  int line = token.y;
  int start = token.x;
  int stop = token.x + token.lexeme.length() - 1;
  AST_Binding *node =
      new AST_Binding(mut, std::string{token.lexeme}, line, start, stop);

  std::optional<AST_Node *> initializer = this->expression();
  node->initializer = initializer;
  return node;
}

AST_Node *Parser::enum_declaration(Token &token) {
  this->pos++; // consume ENUM
  int line = token.y;
  int start = token.x;
  int stop = token.x + token.lexeme.length() - 1;

  if (this->current().type != Token::Type::SYMBOL) {
    this->reporter->new_error(Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT, "Expected a symbol after 'enum'");
    return nullptr;
  }

  // Get the symbol for the enum
  std::string symbol = std::string(this->current().lexeme);
  AST_Enum_Decl *node = new AST_Enum_Decl(symbol, line, start, stop);

  if (!this->peek_consume_if_ignore_newlines(Token::Type::LCURL)) {
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;
    this->reporter->new_error(Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT, "Expected '{' after enum declaration.");
    return nullptr;
  } else
    this->pos++;

  // Take variants
  while (true) {
    if (this->current().type != Token::Type::SYMBOL) {
      Token &tk = this->current();
      int line = tk.y;
      int start = tk.x;
      int stop = tk.x + tk.lexeme.length() - 1;
      this->reporter->new_error(Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT, "Only symbols are allowed in enum declarations");
    } else {
      std::string variant = std::string(this->current().lexeme);
      node->variants.push_back(variant);
    }

    if (this->peek_consume_if(std::vector{Token::Type::NEWLINE, Token::Type::COMMA})) {
      this->pos++;
      continue;
    } else
      break;
  }

  if (!this->peek_consume_if_ignore_newlines(Token::Type::RCURL)) {
    Token &tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;
    this->reporter->new_error(Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT, "Expected '}' to close declaration.");
  } else
    this->pos++;

  return node;
}

AST_Node *Parser::end_statement(AST_Node *stmt) {
  this->skip_to_endof_statement();
  return stmt;
}


AST_Node *Parser::statement() {
  std::cout << "Entering statement: " << this->current().lexeme << std::endl;

  Token &tk = this->current();
  int line = tk.y;
  int start = tk.x;
  int stop = tk.x + tk.lexeme.length() - 1;

  switch (tk.type) {
  case Token::Type::NEWLINE: {
    this->pos++;
    return this->statement();
  }

  case Token::Type::SYMBOL: {
    if (this->peek_consume_if(Token::Type::EQUAL)) {
      // this is a constant binding
      return this->end_statement(this->initialized_binding(tk, false));
    }
    break;
  }

  case Token::Type::RETURN: {
      this->pos++;
      tk = this->current();
      int line = tk.y;
      int start = tk.x;
      int stop = tk.x + tk.lexeme.length() - 1;

      AST_Return *return_node = new AST_Return(line, start, stop);
      
      if (this->peek().type == Token::Type::NEWLINE || this->peek().type ==  Token::Type::SEMICOLON || tk.type == Token::Type::NEWLINE || tk.type == Token::Type::SEMICOLON) {
        return_node->value = std::nullopt;
        // this->pos++;
        return return_node;
      }
        
      AST_Node *node = this->expression();
      return_node->value = node;
      this->skip_to_endof_statement();
      this->pos--;
      return return_node;
    }

  case Token::Type::MUT: {
    this->pos++;
    tk = this->current();
    int line = tk.y;
    int start = tk.x;
    int stop = tk.x + tk.lexeme.length() - 1;

    if (tk.type != Token::Type::SYMBOL) {
      this->reporter->new_error(
          Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT,
          "Expected this to be an identifier after keyword 'mut'");

      this->skip_to_endof_statement();
      this->pos++;
      return nullptr;
    }

    if (this->peek_consume_if(Token::Type::EQUAL)) {
      // this is a mutable binding
      return this->end_statement(this->initialized_binding(tk, true));
    }

    this->reporter->new_error(
        Error::Type::SYNTAX_ERROR, line, start, stop, Error::Flag::ABORT,
        "Expected an '=' after keyword 'mut' and identifier");

    this->skip_to_endof_statement();
    this->pos++;
    return nullptr;
  }

  case Token::Type::ENUM: 
    return this->end_statement(this->enum_declaration(tk));

  case Token::Type::IF:
    return this->end_statement(this->if_stmt(tk));
  }

  AST_Node *expr = this->expression();
  if (expr != nullptr && this->assert_node_type<AST_Call>(expr) ||
      this->assert_node_type<AST_Assignment>(expr)) {
    // This is a normal expression statement
    return expr;
  }
  if (expr == nullptr) {
    std::cerr << "Nullptr expression bro..." << std::endl;
    return nullptr;
  }
  this->reporter->new_error(Error::Type::SYNTAX_ERROR, expr->line, expr->start,
                            expr->stop, Error::Flag::ABORT,
                            "Expressions must be meaningful and/or have "
                            "side-effects to exist on their own like this");
  return nullptr;
}