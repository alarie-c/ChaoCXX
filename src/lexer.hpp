#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>
#include <string_view>
#include <vector>
#include "token.hpp"

class Lexer {
    const std::string& stream;
    uint32_t x, y;

    public:
    std::vector<Token> output;

    Lexer(const std::string &source);
    std::vector<Token>& tokens();
    void scan();

    private:

    // Get the next character and advance. If EOF reached, will return '\0'
    char next();
    
    // Exact same implementation as `Lexer::next()` but does not advance `x`
    char peek();

    bool expect(char ch);

    // Switches between two token types based on the whatever the token is.
    // Token::Type ternary(char peek_condition, Token::Type if_true, Token::Type if_false);
};

#endif
