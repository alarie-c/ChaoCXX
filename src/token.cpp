#include "token.hpp"
#include <iostream>

// Constructor definition
Token::Token(Type type, std::string_view lexeme, uint32_t line, uint32_t offset)
    : type(type), lexeme(lexeme), x(offset), y(line) {}

// Method definition
void Token::print() {
    std::cout << this->y << "; "
              << this->x << "-"
              << this->x + this->lexeme.length() - 1 << "; "
              << this->type << "; "
              << this->lexeme
              << std::endl;
}

// Output operator definition
std::ostream& operator<<(std::ostream& os, const Token::Type& type) {
    static std::map<Token::Type, std::string> types = {
        {Token::Type::Eof, "EOF"},
        {Token::Type::Newline, "NEWLINE"},
        {Token::Type::Doc, "DOC"},

        // Grouping
        {Token::Type::RParen, "R_PAREN"},
        {Token::Type::LParen, "L_PAREN"},
        {Token::Type::RBrac, "R_BRAC"},
        {Token::Type::LBrac, "L_BRAC"},
        {Token::Type::RCurl, "R_CURL"},
        {Token::Type::LCurl, "L_CURL"},

        // Literals
        {Token::Type::Symbol, "SYMBOL"},
        {Token::Type::Number, "NUMBER"},
        {Token::Type::String, "STRING"},

        // Arithmetic operators
        {Token::Type::Plus, "PLUS"},
        {Token::Type::PlusPlus, "PLUS_PLUS"},
        {Token::Type::PlusEqual, "PLUS_EQUAL"},
        {Token::Type::Minus, "MINUS"},
        {Token::Type::MinusMinus, "MINUS_MINUS"},
        {Token::Type::MinusEqual, "MINUS_EQUAL"},
        {Token::Type::Star, "STAR"},
        {Token::Type::StarStar, "STAR_STAR"},
        {Token::Type::StarEqual, "STAR_EQUAL"},
        {Token::Type::Slash, "SLASH"},
        {Token::Type::SlashSlash, "SLASH_SLASH"},
        {Token::Type::SlashEqual, "SLASH_EQUAL"},
        {Token::Type::Modulo, "MODULO"},

        // Comparison operators
        {Token::Type::Equal, "EQUAL"},
        {Token::Type::EqualEqual, "EQUAL_EQUAL"},
        {Token::Type::Bang, "BANG"},
        {Token::Type::BangEqual, "BANG_EQUAL"},
        {Token::Type::Less, "LESS"},
        {Token::Type::LessEqual, "LESS_EQUAL"},
        {Token::Type::More, "MORE"},
        {Token::Type::MoreEqual, "MORE_EQUAL"},
        {Token::Type::Amp, "AMP"},
        {Token::Type::AmpAmp, "AMP_AMP"},
        {Token::Type::Bar, "BAR"},
        {Token::Type::BarBar, "BAR_BAR"},

        // Other operators
        {Token::Type::Arrow, "ARROW"},
        {Token::Type::Colon, "COLON"},
        {Token::Type::Semicolon, "SEMICOLON"},
        {Token::Type::Dot, "DOT"},
        {Token::Type::Comma, "COMMA"},
        {Token::Type::AtSign, "AT_SIGN"},
        {Token::Type::QMark, "Q_MARK"},

        // Keywords
        {Token::Type::Function, "FUNCTION"},
        {Token::Type::Class, "CLASS"},
        {Token::Type::Enum, "ENUM"},
        {Token::Type::Mut, "MUT"},
        {Token::Type::True, "TRUE"},
        {Token::Type::False, "FALSE"},
        {Token::Type::Nil, "NIL"},
        {Token::Type::If, "IF"},
        {Token::Type::Else, "ELSE"},
        {Token::Type::Continue, "CONTINUE"},
        {Token::Type::Break, "BREAK"},
        {Token::Type::For, "FOR"},
        {Token::Type::While, "WHILE"},
        {Token::Type::Case, "CASE"},
        {Token::Type::In, "IN"},
        {Token::Type::Defer, "DEFER"},
        {Token::Type::Return, "RETURN"},
        {Token::Type::From, "FROM"},
        {Token::Type::Import, "IMPORT"},
        {Token::Type::As, "AS"},
    };
    os << types[type];
    return os;
}

std::map<std::string, Token::Type> keywords = {
    {"function", Token::Type::Function},
    {"class", Token::Type::Class},
    {"enum", Token::Type::Enum},
    {"mut", Token::Type::Mut},
    {"true", Token::Type::True},
    {"false", Token::Type::False},
    {"nil", Token::Type::Nil},
    {"switch", Token::Type::Switch},
    {"if", Token::Type::If},
    {"else", Token::Type::Else},
    {"continue", Token::Type::Continue},
    {"break", Token::Type::Break},
    {"for", Token::Type::For},
    {"while", Token::Type::While},
    {"case", Token::Type::Case},
    {"in", Token::Type::In},
    {"defer", Token::Type::Defer},
    {"return", Token::Type::Return},
    {"from", Token::Type::From},
    {"import", Token::Type::Import},
    {"as", Token::Type::As},
};