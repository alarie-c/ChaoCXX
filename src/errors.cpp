#include "errors.hpp"
#include <cstddef>
#include <string>
#include <vector>

Reporter::Reporter(std::string file_name, std::string path)
    : file_name(file_name), path(path) {}
void Reporter::push_error(const Error e) { this->errors.push_back(e); }

Error::Error(Type t, size_t y, size_t x0, size_t x1, Flag flag,
             std::string message)
    : type(t), y(y), x0(x0), x1(x1), message(message), flag(flag) {
  if (flag == EFLAG_ABORT) {
    // If flag is not passed or is 0
    this->flag = Error::Flag::EFLAG_ABORT;
  }
}