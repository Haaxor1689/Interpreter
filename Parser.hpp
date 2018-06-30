#pragma once

#include <optional>

#include "Lexer.hpp"
#include "Evaluator.hpp"
#include "Ast.hpp"

namespace Interpreter {

class Parser {
    std::ifstream source;
    Lexer lexer;
    Token token;
    Ast ast;

public:
    Parser(const std::string& path)
        : source(path), lexer(source), token(lexer.Next()), ast(token, [this]() { token = lexer.Next(); }) {}

    const Ast& Tree() const { return ast; }
    Value Evaluate(const std::string function, const std::list<Value>& arguments = {}) {
        return Evaluator::Evaluate(ast.Root(), function, arguments);
    }
};

} // namespace Interpreter