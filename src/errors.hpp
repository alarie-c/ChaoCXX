#ifndef ERRORS_H
#define ERRORS_H

#include <cstddef>
#include <iostream>
#include <map>
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
  size_t y, x0, x1;
  Flag flag;
  const std::string message;

  Error(Type t, size_t y, size_t x0, size_t x1, Flag flag,
        const std::string message);
};

std::ostream &operator<<(std::ostream &os, const Error &e);

class Reporter {
  const std::string file_name, path;
  const std::string &source;
  std::vector<Error> errors;

public:
  Reporter(const std::string file_name, const std::string path,
           const std::string &source);
  void push_error(const Error &e);
  void print_errors() const;
};

#endif