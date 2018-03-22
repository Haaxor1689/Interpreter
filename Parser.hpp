#pragma once

#include <optional>

#include "Lexer.hpp"
#include "Ast.hpp"

namespace Interpreter {

class Parser {
    Lexer lexer;
    Token token;
    Ast ast;

public:
    Parser(const std::string& path)
        : lexer(path), token(lexer.Next()), ast(token, [this]() { token = lexer.Next(); }) {}

    const Ast& Tree() const { return ast; }
};

} // namespace Interpreter