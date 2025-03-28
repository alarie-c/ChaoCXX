#include <cctype>
#include <map>
#include <string>
#include <vector>

#include "errors.hpp"
#include "lexer.hpp"
#include "token.hpp"

#define LEXEME_SV                                                              \
  (std::string_view(this->stream).substr(start, (1 + this->cursor - start)))

inline Error *lexer_error(Error::Type t, size_t y, size_t x0, size_t x1,
                          Error::Flag flag, std::string message) {
  return new Error(t, y, x0, x1, flag, message);
}

Lexer::Lexer(const std::string &source, Reporter *reporter)
    : stream(source), cursor(0), line(1), reporter(reporter) {}

bool is_hex(char c) {
  return ('0' <= c && c <= '9') || ('A' <= c && c <= 'F') ||
         ('a' <= c && c <= 'f');
}

void Lexer::scan() {
  while (this->cursor < this->stream.length()) {
    char ch = this->stream[this->cursor];
    std::size_t start = this->cursor;

    switch (ch) {
    // Handle whitespaace stuff here
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n': {
      this->output.push_back(Token(Token::Type::NEWLINE,
                                   std::string_view{"\\n"}, this->line, start));
      this->line++;
      break;
    }

    // Handle grouping ops
    case '(':
      this->output.push_back(
          Token(Token::Type::LPAREN, LEXEME_SV, this->line, start));
      break;
    case ')':
      this->output.push_back(
          Token(Token::Type::RPAREN, LEXEME_SV, this->line, start));
      break;
    case '[':
      this->output.push_back(
          Token(Token::Type::LBRAC, LEXEME_SV, this->line, start));
      break;
    case ']':
      this->output.push_back(
          Token(Token::Type::RBRAC, LEXEME_SV, this->line, start));
      break;
    case '{':
      this->output.push_back(
          Token(Token::Type::LCURL, LEXEME_SV, this->line, start));
      break;
    case '}':
      this->output.push_back(
          Token(Token::Type::RCURL, LEXEME_SV, this->line, start));
      break;

    // Handle operators
    case '-': {
      if (expect('>'))
        this->output.push_back(
            Token(Token::Type::ARROW, LEXEME_SV, this->line, start));
      else if (expect('-'))
        this->output.push_back(
            Token(Token::Type::MINUS_MINUS, LEXEME_SV, this->line, start));
      else if (expect('='))
        this->output.push_back(
            Token(Token::Type::MINUS_EQUAL, LEXEME_SV, this->line, start));
      else
        this->output.push_back(
            Token(Token::Type::MINUS, LEXEME_SV, this->line, start));
      break;
    };
    case '+': {
      if (expect('+'))
        this->output.push_back(
            Token(Token::Type::PLUS_PLUS, LEXEME_SV, this->line, start));
      else if (expect('='))
        this->output.push_back(
            Token(Token::Type::PLUS_EQUAL, LEXEME_SV, this->line, start));
      else
        this->output.push_back(
            Token(Token::Type::PLUS, LEXEME_SV, this->line, start));
      break;
    }
    case '*': {
      if (expect('*'))
        this->output.push_back(
            Token(Token::Type::STAR_STAR, LEXEME_SV, this->line, start));
      else if (expect('='))
        this->output.push_back(
            Token(Token::Type::STAR_EQUAL, LEXEME_SV, this->line, start));
      else
        this->output.push_back(
            Token(Token::Type::STAR, LEXEME_SV, this->line, start));
      break;
    }
    case '/': {
      if (expect('/'))
        this->output.push_back(
            Token(Token::Type::SLASH_SLASH, LEXEME_SV, this->line, start));
      else if (expect('='))
        this->output.push_back(
            Token(Token::Type::SLASH_EQUAL, LEXEME_SV, this->line, start));
      else
        this->output.push_back(
            Token(Token::Type::SLASH, LEXEME_SV, this->line, start));
      break;
    }
    case '>': {
      Token::Type t =
          (expect('=')) ? Token::Type::MORE_EQUAL : Token::Type::MORE;
      this->output.push_back(Token(t, LEXEME_SV, this->line, start));
      break;
    };
    case '<': {
      Token::Type t =
          (expect('=')) ? Token::Type::LESS_EQUAL : Token::Type::LESS;
      this->output.push_back(Token(t, LEXEME_SV, this->line, start));
      break;
    };
    case '=': {
      Token::Type t =
          (expect('=')) ? Token::Type::EQUAL_EQUAL : Token::Type::EQUAL;
      this->output.push_back(Token(t, LEXEME_SV, this->line, start));
      break;
    };
    case '!': {
      Token::Type t =
          (expect('=')) ? Token::Type::BANG_EQUAL : Token::Type::BANG;
      this->output.push_back(Token(t, LEXEME_SV, this->line, start));
      break;
    };
    case '&': {
      Token::Type t = (expect('&')) ? Token::Type::AMP_AMP : Token::Type::AMP;
      this->output.push_back(Token(t, LEXEME_SV, this->line, start));
      break;
    };
    case '|': {
      Token::Type t = (expect('|')) ? Token::Type::BAR_BAR : Token::Type::BAR;
      this->output.push_back(Token(t, LEXEME_SV, this->line, start));
      break;
    };

    case ',':
      this->output.push_back(
          Token(Token::Type::COMMA, LEXEME_SV, this->line, start));
      break;
    case '.':
      this->output.push_back(
          Token(Token::Type::DOT, LEXEME_SV, this->line, start));
      break;
    case '?':
      this->output.push_back(
          Token(Token::Type::QMARK, LEXEME_SV, this->line, start));
      break;
    case ':':
      this->output.push_back(
          Token(Token::Type::COLON, LEXEME_SV, this->line, start));
      break;
    case ';':
      this->output.push_back(
          Token(Token::Type::SEMICOLON, LEXEME_SV, this->line, start));
      break;
    case '@':
      this->output.push_back(
          Token(Token::Type::ATSIGN, LEXEME_SV, this->line, start));
      break;
    case '%':
      this->output.push_back(
          Token(Token::Type::MODULO, LEXEME_SV, this->line, start));
      break;

    // Handle comments
    case '#': {
      bool docs = false;
      if (peek() == '\'')
        docs = true;
      else if (peek() == '[') {
        this->output.push_back(
            Token(Token::Type::HASH_BRAC, LEXEME_SV, this->line, start));
        break;
      }

      this->cursor++;
      while (this->stream[this->cursor] != '\n' &&
             this->stream[this->cursor] != '\0')
        this->cursor++;

      if (docs) {
        start += 2; /* ignore the #' at the begining */
        this->output.push_back(
            Token(Token::Type::DOC, LEXEME_SV, this->line, start));
      }
      break;
    }

    case '"': {
      this->cursor++;
      while (peek() != '"' && peek() != '\0')
        this->cursor++;
      
      if (peek() == '\0') {
        this->reporter->new_error(Error::Type::NONTERMINATING_STRLITERAL, this->line, start, this->cursor, Error::Flag::ABORT, "String literal has no closing '\"'");
        this->output.push_back(
          Token(Token::Type::STRING, LEXEME_SV, this->line, start));
        break;
      }
      this->cursor++;
      this->output.push_back(
          Token(Token::Type::STRING, LEXEME_SV, this->line, start));
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
              Token(keywords[std::string(lexeme)], lexeme, this->line, start));
        else
          this->output.push_back(
              Token(Token::Type::SYMBOL, lexeme, this->line, start));

      } else if (isdigit(ch)) {
        if (ch == '0') {
          // Could be octal/hex/binary
          if (peek() == 'x' || peek() == 'X') {
            this->cursor++;

            // Hex requires more tokenizing
            while (true) {
              char now = peek();
              if (isspace(now))
                break;
              else if (now == '\0')
                break;
              else if (now == '_')
                this->cursor++;
              else if (is_hex(now))
                this->cursor++;
              else {
                this->reporter->new_error(
                    Error::Type::SYNTAX_ERROR, this->line, this->cursor+1,
                    this->cursor+1, Error::Flag::ABORT,
                    "Invalid character in hexadecimal number literal");
                break;
              }
            }
            this->output.push_back(
                Token(Token::Type::NUMBER, LEXEME_SV, this->line, start));
            break;
          }

          if (peek() == 'o' || peek() == 'O' || peek() == 'b' ||
              peek() == 'B') {
            char base = peek();
            this->cursor++;

            while (true) {
              char now = peek();
              if (isspace(now))
                break;
              else if (now == '\0')
                break;
              else if (now == '_') {
                this->cursor++;
                continue;
              } else if ((base == 'b' ||
                          base == 'B' && (now == '0' || now == '1')) ||
                         (base == 'o' ||
                          base == 'O' && (now >= '0' && now <= '7'))) {
                this->cursor++;
              } else {
                this->reporter->new_error(
                    Error::Type::SYNTAX_ERROR, this->line, this->cursor+1,
                    this->cursor+1, Error::Flag::ABORT,
                    "Invalid character in binary/octal number literal");
                break;
              }
            }
            this->output.push_back(
                Token(Token::Type::NUMBER, LEXEME_SV, this->line, start));
            break;
          }
        }

        // Tokenize numbers here
        while (isdigit(peek()) || peek() == '_' || peek() == '.')
          this->cursor++;
        this->output.push_back(
            Token(Token::Type::NUMBER, LEXEME_SV, this->line, start));
      } else {
        // This is the catchall for anything that didn't go through the rest of
        // the switch or the else cases afterwards Going to push an error that
        // this character is illegal and just not push it to the output at all
        std::cout << "This is an erroring character" << std::endl;
        this->reporter->new_error(Error::Type::ILLEGAL_CHAR, this->line,
                                  this->cursor, this->cursor,
                                  Error::Flag::ABORT, "Illegal Character");
        break; // !!! Untested
      }
    }
    }

    if (this->stream[this->cursor] == '\0')
      break;
    this->cursor++;
  }
  this->output.push_back(Token(Token::Type::END_OF_FILE,
                               std::string_view{"{ EOF }"}, this->line,
                               this->cursor));
}

char Lexer::next() {
  if (this->cursor >= this->stream.length())
    return '\0';
  return this->stream[this->cursor++];
}

char Lexer::peek() const {
  if (this->cursor + 1 >= this->stream.length())
    return '\0';
  return this->stream[this->cursor + 1];
}

bool Lexer::expect(char ch) {
  if (peek() == ch) {
    this->cursor++;
    return true;
  }
  return false;
}