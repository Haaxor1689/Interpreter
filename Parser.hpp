#pragma once

#include <optional>

#include "Lexer.hpp"
#include "Evaluator.hpp"
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
    Value Evaluate(const std::string function, const std::list<Value>& arguments = {}) {
        //try {
            return Evaluator::Evaluate(ast.Root(), function, arguments);
        //} catch (const std::exception& ex) {
        //    std::cerr << "Evaluation ended with uncaught exception with message: " << ex.what() << std::endl;
        //    return Value();
        //}
    }
};

} // namespace Interpreter