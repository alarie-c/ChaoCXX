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
void Reporter::push_error(const Error &e) { this->errors.emplace_back(e); }

Error::Error(Type t, size_t y, size_t x0, size_t x1, Flag flag,
             const std::string message)
    : type(t), y(y), x0(x0), x1(x1), message(std::move(message)), flag(flag) {
  if (flag == EFLAG_ABORT) {
    // If flag is not passed or is 0
    this->flag = Error::Flag::EFLAG_ABORT;
  }
}

void Reporter::print_errors() const {
  int n_errors = 1;
  for (Error e : this->errors) {
    // Do some bounds checking
    if (e.x1 > this->source.length()) {
      std::cerr << "ERROR End of error reporter substring is logner than "
                   "source code!"
                << std::endl;
      continue;
    }

    // Getting the start and end of this line+
    size_t ln_start = 0;
    size_t ln_end = this->source.length();

    // Decrement backwards to find the beginning of this line
    for (size_t i = e.x0; i > 0; i--) {
      if (this->source[i - 1] == '\n') {
        ln_start = i;
        break;
      }
    }

    // Increment to find the end of this line
    for (size_t i = e.x1; i < this->source.length(); i++) {
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
    std::string_view buffer = this->source.substr(ln_start, ln_end - ln_start);

    // Get the whitespace for the underline output
    size_t ws_n = e.x0 - ln_start; // Difference from the start of the line to
                                   // the start of the underlined region
    size_t underline_len = e.x1 - e.x0 + 1;

    // Construct the underlining string
    std::string ws = std::string(ws_n, ' ');
    std::string underline = ws + std::string(underline_len, '^');

    std::cerr << "n_errors: " << n_errors << "\n";
    std::cerr << "path: " << this->path << "\n";
    std::cerr << "e: " << e << "\n";
    std::cerr << "e.y: " << e.y << "\n";
    std::cerr << "buffer: [" << buffer << "]\n";
    std::cerr << "underline: [" << underline << "]\n";
    std::cerr << "message: " << e.message << "\n";
    std::cerr << std::endl;

    std::cout << "\n[" << n_errors << "] " << this->path << " " << e
              << " on line " << e.y << "\n~\n~ " << buffer << "\n~ "
              << underline << "\n"
              << e.message << "\n"
              << std::endl;
    n_errors++;
  }
}

std::ostream &operator<<(std::ostream &os, const Error &e) {
  static std::map<Error::Type, std::string> types = {
      {Error::Type::ERROR_ILLEGAL_CHAR, "Illegal Character"},
      {Error::Type::ERROR_EXPECTED_EXPRESSION, "Expected Expression"},
      {Error::Type::ERROR_SYNTAX_ERROR, "Syntax Error"},
      {Error::Type::ERROR_NONTERMINATING_STRLITERAL,
       "Non-terminating String Literal"},
  };
  os << types[e.type];
  return os;
}