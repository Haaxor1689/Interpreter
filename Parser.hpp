#pragma once

#include <filesystem>

#include "Lexer.hpp"
#include "Ast.hpp"

namespace Interpreter {
class Parser {
    Lexer lexer;
    Token token;

public:
    Ast ast;
    Parser(const std::filesystem::path& path) : lexer(path), token(lexer.Next()) {
        try {
            ast = Ast(token, [this](){ token = lexer.Next(); });
        } catch (const ParseError& err) {
            std::cerr << "\033[31m" << err.what() << "\033[0m" << std::endl;
        }
    }
};
}