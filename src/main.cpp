#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <streambuf>
#include <string>

#include "errors.hpp"
#include "lexer.hpp"
#include "token.hpp"

const char *FILE_PATH = "../main.chao";

std::optional<std::string> read_file(const char *path) {
  // First check if the file exists before we try to open it.
  if (!std::filesystem::exists(path)) {
    std::cerr << "File path '" << path << "' does not exist!";
    return std::nullopt;
  }

  // Then check to see if we actually opened the file
  // If the file exists but wasn't opened, there's probably an OS
  // permissions issue with this file
  std::ifstream file(path);
  if (!file) {
    std::cerr << "There was an error opening the file '" << path << "'!";
    return std::nullopt;
  }

  // Use a streambuffer iterator from the file and create a string from it
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  return content;
}

// Move the tokenize functionality out of main() so that the lexer only
// exists on the stack for as long as we need it to
std::vector<Token> tokenize(std::string &source, Reporter *reporter) {
  Lexer lexer = Lexer(source, reporter);
  lexer.scan();
  std::vector<Token> tokens = lexer.output;
  return tokens;
}

int main() {
  auto source = read_file(FILE_PATH);
  if (!source)
    return -1;

  // Allocate this on the heap so we can leave more stack space for AST nodes
  Reporter *reporter = new Reporter("main.chao", FILE_PATH, *source);
  std::vector<Token> tokens = tokenize(*source, reporter);

  for (auto t : tokens)
    t.print();

  reporter->print_errors();

  return 0;
}