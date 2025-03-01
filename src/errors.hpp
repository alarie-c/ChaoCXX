#ifndef ERRORS_H
#define ERRORS_H

#include <cstddef>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct Error {
  enum Type {
    ILLEGAL_CHAR,
    NONTERMINATING_STRLITERAL,
    EXPECTED_EXPRESSION,
    SYNTAX_ERROR,
    TOO_MANY_PARAMS,
    TOO_MANY_ARGS,
    TOO_MANY_MEMBERS,
    TOO_MANY_VARIANTS,
  };

  enum Flag {
    ABORT = 0,
    WARNING,
    SUGGESTION,
  };

  Type type;
  size_t line, x0, x1;
  Flag flag;
  const std::string message;

  Error(Type t, size_t line, size_t x0, size_t x1, Flag flag,
        std::string message);
};

std::ostream &operator<<(std::ostream &os, const Error &e);

class Reporter {
  const std::string file_name, path;
  const std::string &source;
  std::vector<Error *> errors;

public:
  Reporter(const std::string file_name, const std::string path,
           const std::string &source);
  void new_error(Error::Type type, size_t line, size_t start, size_t end,
                 Error::Flag flag, std::string message);
  void print_errors() const;
};

#endif