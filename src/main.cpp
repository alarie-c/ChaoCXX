#include <iostream>
#include <string>
#include <fstream>
#include <streambuf> 
#include <filesystem>
#include <optional>

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
    std::string content((
        std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    return content;
}

int main() {
    auto source = read_file(FILE_PATH);
    if (!source)
        return -1;
    
    // std::string s = {*source};
    Lexer lexer = Lexer(*source);

    lexer.scan();

    for (auto t : lexer.tokens())
        t.print();
    
    return 0;
}