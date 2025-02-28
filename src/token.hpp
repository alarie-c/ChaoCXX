#ifndef TOKEN_H
#define TOKEN_H

#include <iostream>
#include <map>
#include <string_view>

// Pretty self explanatory here
// The location of the token is tracked by the `x`, `y`, and `lexeme` members
// `x` is the index of the first character of the lexeme
// `y` is the line number, starts at 1
// `lexeme` stores the lexeme, but also we use `length()` method to get the end
// of the token e.g. `x + lexeme.length() - 1`
struct Token {
  enum Type {
    // OTHER
    END_OF_FILE,
    NEWLINE,
    DOC,

    // GROUPING
    LPAREN,
    RPAREN,
    LBRAC,
    RBRAC,
    LCURL,
    RCURL,

    // LITERALS
    SYMBOL,
    NUMBER,
    STRING,

    // ARITHMETIC OPERATORS
    PLUS,
    PLUS_PLUS,
    PLUS_EQUAL,
    MINUS,
    MINUS_MINUS,
    MINUS_EQUAL,
    STAR,
    STAR_STAR,
    STAR_EQUAL,
    SLASH,
    SLASH_SLASH,
    SLASH_EQUAL,
    MODULO,

    // COMPARISON OPERATORS
    EQUAL,
    EQUAL_EQUAL,
    BANG,
    BANG_EQUAL,
    LESS,
    LESS_EQUAL,
    MORE,
    MORE_EQUAL,
    AMP,
    AMP_AMP,
    BAR,
    BAR_BAR,

    // OTHER OPERATORS
    ARROW,
    COLON,
    SEMICOLON,
    DOT,
    COMMA,
    ATSIGN,
    QMARK,

    // KEYWORDS
    FUNCTION,
    CLASS,
    ENUM,
    MUT,
    TRUE,
    FALSE,
    NIL,
    SWITCH,
    IF,
    ELSE,
    CONTINUE,
    BREAK,
    FOR,
    WHILE,
    CASE,
    IN,
    DEFER,
    RETURN,
    FROM,
    IMPORT,
    AS,
  };

  Type type;
  int x, y;
  std::string_view lexeme;

  Token(Type type, std::string_view lexeme, int line, int offset);
  void print() const;
};

// Operator overload `<<` for `Token`
std::ostream &operator<<(std::ostream &os, const Token::Type &type);

// For matching `string_views` to keyword TokenTypes
extern std::map<std::string, Token::Type> keywords;

#endif