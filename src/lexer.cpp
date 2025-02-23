#include <cctype>
#include <map>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "token.hpp"

#define LEXEME_SV                                                              \
  (std::string_view(this->stream).substr(start, (1 + this->x - start)))

Lexer::Lexer(const std::string &source) : stream(source), x(0), y(1) {}

std::vector<Token> &Lexer::tokens() { return this->output; }

void Lexer::scan() {
  while (this->x < this->stream.length()) {
    char ch = this->stream[this->x];
    std::size_t start = this->x;

    switch (ch) {
    // Handle whitespaace stuff here
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n': {
      this->output.push_back(
          Token(Token::Type::NEWLINE, std::string_view{"\\n"}, this->y, start));
      this->y++;
      break;
    }

    // Handle grouping ops
    case '(':
      this->output.push_back(
          Token(Token::Type::LPAREN, LEXEME_SV, this->y, start));
      break;
    case ')':
      this->output.push_back(
          Token(Token::Type::RPAREN, LEXEME_SV, this->y, start));
      break;
    case '[':
      this->output.push_back(
          Token(Token::Type::LBRAC, LEXEME_SV, this->y, start));
      break;
    case ']':
      this->output.push_back(
          Token(Token::Type::RBRAC, LEXEME_SV, this->y, start));
      break;
    case '{':
      this->output.push_back(
          Token(Token::Type::LCURL, LEXEME_SV, this->y, start));
      break;
    case '}':
      this->output.push_back(
          Token(Token::Type::RCURL, LEXEME_SV, this->y, start));
      break;

    // Handle operators
    case '-': {
      if (expect('>'))
        this->output.push_back(
            Token(Token::Type::ARROW, LEXEME_SV, this->y, start));
      else if (expect('-'))
        this->output.push_back(
            Token(Token::Type::MINUS_MINUS, LEXEME_SV, this->y, start));
      else if (expect('='))
        this->output.push_back(
            Token(Token::Type::MINUS_EQUAL, LEXEME_SV, this->y, start));
      else
        this->output.push_back(
            Token(Token::Type::MINUS, LEXEME_SV, this->y, start));
      break;
    };
    case '+': {
      if (expect('+'))
        this->output.push_back(
            Token(Token::Type::PLUS_PLUS, LEXEME_SV, this->y, start));
      else if (expect('='))
        this->output.push_back(
            Token(Token::Type::PLUS_EQUAL, LEXEME_SV, this->y, start));
      else
        this->output.push_back(
            Token(Token::Type::PLUS, LEXEME_SV, this->y, start));
      break;
    }
    case '*': {
      if (expect('*'))
        this->output.push_back(
            Token(Token::Type::STAR_STAR, LEXEME_SV, this->y, start));
      else if (expect('='))
        this->output.push_back(
            Token(Token::Type::STAR_EQUAL, LEXEME_SV, this->y, start));
      else
        this->output.push_back(
            Token(Token::Type::STAR, LEXEME_SV, this->y, start));
      break;
    }
    case '/': {
      if (expect('/'))
        this->output.push_back(
            Token(Token::Type::SLASH_SLASH, LEXEME_SV, this->y, start));
      else if (expect('='))
        this->output.push_back(
            Token(Token::Type::SLASH_EQUAL, LEXEME_SV, this->y, start));
      else
        this->output.push_back(
            Token(Token::Type::SLASH, LEXEME_SV, this->y, start));
      break;
    }
    case '>': {
      Token::Type t =
          (expect('=')) ? Token::Type::MORE_EQUAL : Token::Type::MORE;
      this->output.push_back(Token(t, LEXEME_SV, this->y, start));
      break;
    };
    case '<': {
      Token::Type t =
          (expect('=')) ? Token::Type::LESS_EQUAL : Token::Type::LESS;
      this->output.push_back(Token(t, LEXEME_SV, this->y, start));
      break;
    };
    case '=': {
      Token::Type t =
          (expect('=')) ? Token::Type::EQUAL_EQUAL : Token::Type::EQUAL;
      this->output.push_back(Token(t, LEXEME_SV, this->y, start));
      break;
    };
    case '!': {
      Token::Type t =
          (expect('=')) ? Token::Type::BANG_EQUAL : Token::Type::BANG;
      this->output.push_back(Token(t, LEXEME_SV, this->y, start));
      break;
    };
    case '&': {
      Token::Type t = (expect('&')) ? Token::Type::AMP_AMP : Token::Type::AMP;
      this->output.push_back(Token(t, LEXEME_SV, this->y, start));
      break;
    };
    case '|': {
      Token::Type t = (expect('|')) ? Token::Type::BAR_BAR : Token::Type::BAR;
      this->output.push_back(Token(t, LEXEME_SV, this->y, start));
      break;
    };

    case ',':
      this->output.push_back(
          Token(Token::Type::COMMA, LEXEME_SV, this->y, start));
      break;
    case '.':
      this->output.push_back(
          Token(Token::Type::DOT, LEXEME_SV, this->y, start));
      break;
    case '?':
      this->output.push_back(
          Token(Token::Type::QMARK, LEXEME_SV, this->y, start));
      break;
    case ':':
      this->output.push_back(
          Token(Token::Type::COLON, LEXEME_SV, this->y, start));
      break;
    case ';':
      this->output.push_back(
          Token(Token::Type::SEMICOLON, LEXEME_SV, this->y, start));
      break;
    case '@':
      this->output.push_back(
          Token(Token::Type::ATSIGN, LEXEME_SV, this->y, start));
      break;

    // Handle comments
    case '#': {
      bool docs = false;
      if (peek() == '\'')
        docs = true;

      while (peek() != '\n')
        this->x++;
      if (docs) {
        start += 2; /* ignore the #' at the begining */
        this->output.push_back(
            Token(Token::Type::DOC, LEXEME_SV, this->y, start));
      }
      break;
    }

    case '"': {
      this->x++;
      while (peek() != '"')
        this->x++;
      this->x++;
      this->output.push_back(
          Token(Token::Type::STRING, LEXEME_SV, this->y, start));
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
              Token(keywords[std::string(lexeme)], lexeme, this->y, start));
        else
          this->output.push_back(
              Token(Token::Type::SYMBOL, lexeme, this->y, start));

      } else if (isdigit(ch)) {
        // Tokenize numbers here
        while (isdigit(peek()) || peek() == '_' || peek() == '.')
          next(); /* unused return */
        this->output.push_back(
            Token(Token::Type::NUMBER, LEXEME_SV, this->y, start));
      }
    }
    }

    if (this->stream[this->x] == '\0') {
      this->output.push_back(Token(Token::Type::END_OF_FILE,
                                   std::string_view{"{ EOF }"}, this->y,
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