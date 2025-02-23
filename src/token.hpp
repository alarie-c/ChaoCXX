#ifndef TOKEN_H
#define TOKEN_H

#include <string_view>
#include <stdint.h>
#include <iostream>
#include <map>

// Pretty self explanatory here
// The location of the token is tracked by the `x`, `y`, and `lexeme` members
// `x` is the index of the first character of the lexeme
// `y` is the line number, starts at 1
// `lexeme` stores the lexeme, but also we use `length()` method to get the end of the token
// e.g. `x + lexeme.length() - 1`
struct Token {
    enum class Type {
        // Other
        Eof = 0,
        Newline,
        Doc,

        // Grouping
        LParen,
        RParen,
        LBrac,
        RBrac,
        LCurl,
        RCurl,
        
        // Literals
        Symbol,
        Number,
        String,
        
        // Arithmetic operators
        Plus,
        PlusPlus,
        PlusEqual,
        Minus,
        MinusMinus,
        MinusEqual,
        Star,
        StarStar,
        StarEqual,
        Slash,
        SlashSlash,
        SlashEqual,
        Modulo,

        // Comparison operators
        Equal,
        EqualEqual,
        Bang,
        BangEqual,
        Less,
        LessEqual,
        More,
        MoreEqual,
        Amp,
        AmpAmp,
        Bar,
        BarBar,

        // Other operators
        Arrow,
        Colon,
        Semicolon,
        Dot,
        Comma,
        AtSign,
        QMark,

        // Keywords
        Function,
        Class,
        Enum,
        Mut,
        True,
        False,
        Nil,
        Switch,
        If,
        Else,
        Continue,
        Break,
        For,
        While,
        Case,
        In,
        Defer,
        Return,
        From,
        Import,
        As,

    };

    Type type;
    uint32_t x, y;
    std::string_view lexeme;

    Token(Type type, std::string_view lexeme, uint32_t line, uint32_t offset);
    void print();
};

std::ostream& operator<<(std::ostream& os, const Token::Type& type);

// For matching `string_views` to keyword TokenTypes
extern std::map<std::string, Token::Type> keywords;

#endif