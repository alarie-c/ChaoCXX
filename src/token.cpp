#include "token.hpp"
#include <iostream>

// Constructor definition
Token::Token(Type type, std::string_view lexeme, int line, int offset)
    : type(type), lexeme(lexeme), x(offset), y(line) {}

// Method definition
void Token::print() const {
  std::cout << this->y << "; " << this->x << "-"
            << this->x + this->lexeme.length() - 1 << "; " << this->type << "; "
            << this->lexeme << std::endl;
}

// Output operator definition
std::ostream &operator<<(std::ostream &os, const Token::Type &type) {
  static std::map<Token::Type, std::string> types = {
      {Token::Type::END_OF_FILE, "EOF"},
      {Token::Type::NEWLINE, "NEWLINE"},
      {Token::Type::DOC, "DOC"},
      {Token::Type::HASH_BRAC, "HASH_BRAC"},

      // Grouping
      {Token::Type::RPAREN, "R_PAREN"},
      {Token::Type::LPAREN, "L_PAREN"},
      {Token::Type::RBRAC, "R_BRAC"},
      {Token::Type::LBRAC, "L_BRAC"},
      {Token::Type::RCURL, "R_CURL"},
      {Token::Type::LCURL, "L_CURL"},

      // Literals
      {Token::Type::SYMBOL, "SYMBOL"},
      {Token::Type::NUMBER, "NUMBER"},
      {Token::Type::STRING, "STRING"},

      // Arithmetic operators
      {Token::Type::PLUS, "PLUS"},
      {Token::Type::PLUS_PLUS, "PLUS_PLUS"},
      {Token::Type::PLUS_EQUAL, "PLUS_EQUAL"},
      {Token::Type::MINUS, "MINUS"},
      {Token::Type::MINUS_MINUS, "MINUS_MINUS"},
      {Token::Type::MINUS_EQUAL, "MINUS_EQUAL"},
      {Token::Type::STAR, "STAR"},
      {Token::Type::STAR_STAR, "STAR_STAR"},
      {Token::Type::STAR_EQUAL, "STAR_EQUAL"},
      {Token::Type::SLASH, "SLASH"},
      {Token::Type::SLASH_SLASH, "SLASH_SLASH"},
      {Token::Type::SLASH_EQUAL, "SLASH_EQUAL"},
      {Token::Type::MODULO, "MODULO"},

      // Comparison operators
      {Token::Type::EQUAL, "EQUAL"},
      {Token::Type::EQUAL_EQUAL, "EQUAL_EQUAL"},
      {Token::Type::BANG, "BANG"},
      {Token::Type::BANG_EQUAL, "BANG_EQUAL"},
      {Token::Type::LESS, "LESS"},
      {Token::Type::LESS_EQUAL, "LESS_EQUAL"},
      {Token::Type::MORE, "MORE"},
      {Token::Type::MORE_EQUAL, "MORE_EQUAL"},
      {Token::Type::AMP, "AMP"},
      {Token::Type::AMP_AMP, "AMP_AMP"},
      {Token::Type::BAR, "BAR"},
      {Token::Type::BAR_BAR, "BAR_BAR"},

      // Other operators
      {Token::Type::ARROW, "ARROW"},
      {Token::Type::COLON, "COLON"},
      {Token::Type::SEMICOLON, "SEMICOLON"},
      {Token::Type::DOT, "DOT"},
      {Token::Type::COMMA, "COMMA"},
      {Token::Type::ATSIGN, "AT_SIGN"},
      {Token::Type::QMARK, "Q_MARK"},

      // Keywords
      {Token::Type::FUNCTION, "FUNCTION"},
      {Token::Type::CLASS, "CLASS"},
      {Token::Type::ENUM, "ENUM"},
      {Token::Type::MUT, "MUT"},
      {Token::Type::TRUE, "TRUE"},
      {Token::Type::FALSE, "FALSE"},
      {Token::Type::NIL, "NIL"},
      {Token::Type::IF, "IF"},
      {Token::Type::ELSE, "ELSE"},
      {Token::Type::CONTINUE, "CONTINUE"},
      {Token::Type::BREAK, "BREAK"},
      {Token::Type::FOR, "FOR"},
      {Token::Type::WHILE, "WHILE"},
      {Token::Type::CASE, "CASE"},
      {Token::Type::IN, "IN"},
      {Token::Type::DEFER, "DEFER"},
      {Token::Type::RETURN, "RETURN"},
      {Token::Type::FROM, "FROM"},
      {Token::Type::IMPORT, "IMPORT"},
      {Token::Type::AS, "AS"},
  };
  os << types[type];
  return os;
}

std::map<std::string, Token::Type> keywords = {
    {"function", Token::Type::FUNCTION},
    {"class", Token::Type::CLASS},
    {"enum", Token::Type::ENUM},
    {"mut", Token::Type::MUT},
    {"true", Token::Type::TRUE},
    {"false", Token::Type::FALSE},
    {"nil", Token::Type::NIL},
    {"switch", Token::Type::SWITCH},
    {"if", Token::Type::IF},
    {"else", Token::Type::ELSE},
    {"continue", Token::Type::CONTINUE},
    {"break", Token::Type::BREAK},
    {"for", Token::Type::FOR},
    {"while", Token::Type::WHILE},
    {"case", Token::Type::CASE},
    {"in", Token::Type::IN},
    {"defer", Token::Type::DEFER},
    {"return", Token::Type::RETURN},
    {"from", Token::Type::FROM},
    {"import", Token::Type::IMPORT},
    {"as", Token::Type::AS},
};