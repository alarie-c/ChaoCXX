#ifndef ERRORS_H
#define ERRORS_H

#include <cstddef>
#include <string>
#include <vector>

struct Error {
  enum Type {
    ERROR_ILLEGAL_CHAR,
    ERROR_NONTERMINATING_STRLITERAL,
    ERROR_EXPECTED_EXPRESSION,
    ERROR_SYNTAX_ERROR,
  };

  enum Flag {
    EFLAG_ABORT = 0,
    EFLAG_WARNING,
    EFLAG_SUGGESTION,
  };

  Type type;
  Flag flag;
  size_t y, x0, x1;
  std::string message;

  Error(Type t, size_t y, size_t x0, size_t x1, Flag flag, std::string message);
  void print() const;
};

class Reporter {
  std::string file_name, path;
  std::vector<Error> errors;

public:
  Reporter(std::string file_name, std::string path);
  void push_error(const Error e);
};

#endif