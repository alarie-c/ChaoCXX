#include <string>
#include <vector>
#include <stdint.h>
#include <optional>
#include <cctype>
#include <map>

#include "lexer.hpp"
#include "token.hpp"

#define LEXEME_SV (std::string_view(this->stream).substr(start, (1 + this->x - start)))

Lexer::Lexer(const std::string &source)
: stream(source), x(0), y(1) {}

std::vector<Token>& Lexer::tokens() { return this->output; }

void Lexer::scan() {
    while (this->x < this->stream.length()) {
        char ch = this->stream[this->x];
        uint32_t start = this->x;

        switch (ch) {
        // Handle whitespaace stuff here
        case ' ': case '\r': case '\t': break;
        case '\n': {
            this->output.push_back(
                Token(Token::Type::Newline,
                std::string_view {"\\n"},
                this->y, start));
            this->y++;
            break;
        }

        // Handle grouping ops
        case '(': 
            this->output.push_back(Token(Token::Type::LParen, LEXEME_SV, this->y, start)); break;
        case ')':
            this->output.push_back(Token(Token::Type::RParen, LEXEME_SV, this->y, start)); break;
        case '[':
            this->output.push_back(Token(Token::Type::LBrac, LEXEME_SV, this->y, start)); break;
        case ']':
            this->output.push_back(Token(Token::Type::RBrac, LEXEME_SV, this->y, start)); break;
        case '{':
            this->output.push_back(Token(Token::Type::LCurl, LEXEME_SV, this->y, start)); break;
        case '}':
            this->output.push_back(Token(Token::Type::RCurl, LEXEME_SV, this->y, start)); break;
        
        // Handle operators
        case '-': {
            if (expect('>'))
                this->output.push_back(Token(Token::Type::Arrow, LEXEME_SV, this->y, start));
            else if (expect('-'))
                this->output.push_back(Token(Token::Type::MinusMinus, LEXEME_SV, this->y, start));
            else if (expect('='))
                this->output.push_back(Token(Token::Type::MinusEqual, LEXEME_SV, this->y, start));
            else
                this->output.push_back(Token(Token::Type::Minus, LEXEME_SV, this->y, start));
            break;
        };
        case '+': {
            if (expect('+'))
                this->output.push_back(Token(Token::Type::PlusPlus, LEXEME_SV, this->y, start));
            else if (expect('='))
                this->output.push_back(Token(Token::Type::PlusEqual, LEXEME_SV, this->y, start));
            else
                this->output.push_back(Token(Token::Type::Plus, LEXEME_SV, this->y, start));
            break;
        }
        case '*': {
            if (expect('*'))
                this->output.push_back(Token(Token::Type::StarStar, LEXEME_SV, this->y, start));
            else if (expect('='))
                this->output.push_back(Token(Token::Type::StarEqual, LEXEME_SV, this->y, start));
            else
                this->output.push_back(Token(Token::Type::Star, LEXEME_SV, this->y, start));
            break;
        }
        case '/': {
            if (expect('/'))
                this->output.push_back(Token(Token::Type::SlashSlash, LEXEME_SV, this->y, start));
            else if (expect('='))
                this->output.push_back(Token(Token::Type::SlashEqual, LEXEME_SV, this->y, start));
            else
                this->output.push_back(Token(Token::Type::Slash, LEXEME_SV, this->y, start));
            break;
        }
        case '>': {
            Token::Type t = (expect('=')) ? Token::Type::MoreEqual : Token::Type::More;
            this->output.push_back(Token(t, LEXEME_SV, this->y, start));
            break;
        };
        case '<': {
            Token::Type t = (expect('=')) ? Token::Type::LessEqual : Token::Type::Less;
            this->output.push_back(Token(t, LEXEME_SV, this->y, start));
            break;
        };
        case '=': {
            Token::Type t = (expect('=')) ? Token::Type::EqualEqual : Token::Type::Equal;
            this->output.push_back(Token(t, LEXEME_SV, this->y, start));
            break;
        };
        case '!': {
            Token::Type t = (expect('=')) ? Token::Type::BangEqual : Token::Type::Bang;
            this->output.push_back(Token(t, LEXEME_SV, this->y, start));
            break;
        };
        case '&': {
            Token::Type t = (expect('&')) ? Token::Type::AmpAmp : Token::Type::Amp;
            this->output.push_back(Token(t, LEXEME_SV, this->y, start));
            break;
        };
        case '|': {
            Token::Type t = (expect('|')) ? Token::Type::BarBar : Token::Type::Bar;
            this->output.push_back(Token(t, LEXEME_SV, this->y, start));
            break;
        };

        case ',':
            this->output.push_back(Token(Token::Type::Comma, LEXEME_SV, this->y, start)); break;
        case '.':
            this->output.push_back(Token(Token::Type::Dot, LEXEME_SV, this->y, start)); break;
        case '?':
            this->output.push_back(Token(Token::Type::QMark, LEXEME_SV, this->y, start)); break;
        case ':':
            this->output.push_back(Token(Token::Type::Colon, LEXEME_SV, this->y, start)); break;
        case ';':
            this->output.push_back(Token(Token::Type::Semicolon, LEXEME_SV, this->y, start)); break;
        case '@':
            this->output.push_back(Token(Token::Type::AtSign, LEXEME_SV, this->y, start)); break;

        case '"': {
            this->x++;
            while (peek() != '"')
                this->x++;
            this->x++;
            this->output.push_back(Token(Token::Type::String, LEXEME_SV, this->y, start));
            break;
        }

        // Handle literals and keywords
        default: {
            if (isalpha(ch) || ch == '_') {
                // Tokenize symbols here
                while (isalnum(peek()) || peek() == '_')
                    next(); /* unused return */
                
                // Check to see if this is a keyword
                std::string_view lexeme = LEXEME_SV;
                if (keywords.count(std::string(lexeme)) != 0)
                    this->output.push_back(
                        Token(keywords[std::string(lexeme)],
                        lexeme,
                        this->y, start));
                else
                    this->output.push_back(
                        Token(Token::Type::Symbol,
                        lexeme,
                        this->y, start));
            
            } else if (isdigit(ch)) {
                // Tokenize numbers here
                while (isdigit(peek()) || peek() == '_' || peek() == '.')
                    next(); /* unused return */
                this->output.push_back(
                    Token(Token::Type::Number,
                    LEXEME_SV,
                    this->y, start));
            }
        }
        }

        if (this->stream[this->x] == '\0') {
            this->output.push_back(
                Token(Token::Type::Eof, 
                std::string_view {"{ EOF }"},
                this->y,
                this->x));
            return;
        }    
        this->x++;
    }
}

char Lexer::next() {
    if (this->x >= this->stream.length())
        return '\0';
    return this->stream[this->x++];
}

char Lexer::peek() {
    if (this->x + 1 >= this->stream.length())
        return '\0';
    return this->stream[this->x + 1];
}

bool Lexer::expect(char ch) {
    if (peek() == ch) {
        this->x++;
        return true;
    }
    return false;
}