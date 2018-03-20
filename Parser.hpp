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
    Parser(const std::filesystem::path& path)
        : lexer(path), token(lexer.Next()) {
        ast = Ast(token, [this]() { token = lexer.Next(); });
    }
};

} // namespace Interpreter