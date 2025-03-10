#include "errors.hpp"
#include <cstddef>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <vector>

Reporter::Reporter(const std::string file_name, const std::string path,
                   const std::string &source)
    : file_name(file_name), path(path), source(source) {}

void Reporter::new_error(Error::Type type, size_t line, size_t start,
                         size_t end, Error::Flag flag, std::string message) {
  std::cout << "Pushing a new error: " << type << std::endl;

  Error *error = new Error(type, line, start, end, flag, message);
  std::cout << error->message << std::endl;
  this->errors.push_back(error);
};

Error::Error(Type t, size_t line, size_t x0, size_t x1, Flag flag,
             std::string message)
    : type(t), line(line), x0(x0), x1(x1), flag(flag),
      message(std::move(message)) {}

void Reporter::print_errors() const {
  int n_errors = 1;
  for (Error *e : this->errors) {
    // Do some bounds checking
    if (e->x1 > this->source.length()) {
      std::cerr << "ERROR End of error reporter substring is longer than "
                   "source code!"
                << std::endl;
      continue;
    }

    // Getting the start and end of this line+
    size_t ln_start = 0;
    size_t ln_end = this->source.length();

    // Decrement backwards to find the beginning of this line
    for (size_t i = e->x0; i > 0; i--) {
      if (this->source[i - 1] == '\n') {
        ln_start = i;
        break;
      }
    }

    // Increment to find the end of this line
    for (size_t i = e->x1; i < this->source.length(); i++) {
      if (this->source[i] == '\n')
        break;
      ln_end = i + 1;
    }

    // Store the entire line contents
    if (ln_start >= ln_end) {
      std::cerr << "ERROR ln_start >= ln_end (" << ln_start << " >= " << ln_end
                << ")" << std::endl;
      continue;
    }

    // Get the entire contents of the file
    std::string buffer = this->source.substr(ln_start, ln_end - ln_start);

    // Get the whitespace for the underline output
    size_t ws_n = e->x0 - ln_start; // Difference from the start of the line to
                                    // the start of the underlined region
    size_t underline_len = e->x1 - e->x0 + 1;

    // Construct the underlining string
    std::string ws = std::string(ws_n, ' ');
    std::string underline = ws + std::string(underline_len, '^');

    std::cout << TERM_ESC << TERMCOL_ERROR << "\nerror " << TERM_RESET << this->path << " " << e->type << " on line " << e->line << "\n~\n~ " << buffer << "\n~ " << TERM_ESC << TERMCOL_HIGHLIGHT << underline << TERM_RESET << "\n" << TERM_ESC << TERMCOL_MESSAGE << e->message << TERM_RESET << "\n" << std::endl;
    n_errors++;
  }
}

std::ostream &operator<<(std::ostream &os, const Error::Type &t) {
  static std::map<Error::Type, std::string> types = {
      {Error::Type::ILLEGAL_CHAR, "Illegal Character"},
      {Error::Type::EXPECTED_EXPRESSION, "Expected Expression"},
      {Error::Type::SYNTAX_ERROR, "Syntax Error"},
      {Error::Type::NONTERMINATING_STRLITERAL,
       "Non-terminating String Literal"},
      {Error::Type::TOO_MANY_ARGS, "Too Many Arguments"},
      {Error::Type::TOO_MANY_PARAMS, "Too Many Parameters"},
      {Error::Type::TOO_MANY_MEMBERS, "Too Many Members"},
      {Error::Type::TOO_MANY_VARIANTS, "Too Many Variants"},
  };
  os << TERM_ESC << TERMCOL_MESSAGE << types[t] << TERM_RESET;
  return os;
}