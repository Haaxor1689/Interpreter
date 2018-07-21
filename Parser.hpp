#pragma once

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

    Value Evaluate(const std::string function, int argc, char* argv[]) const {
        std::list<Value> parsedArguments; 
        for (auto i = 0; i < argc; ++i) {
            std::string argument(argv[i]);
            char* end;
            if (argument == "true" || argument == "True") {
                parsedArguments.emplace_back(true);
            } else if (argument == "false" || argument == "False") {
                parsedArguments.emplace_back(false);
            } else if (double num = std::strtod(&argument[0], &end); end == &argument[argument.size()]) {
                parsedArguments.emplace_back(num);
            } else {
                parsedArguments.emplace_back(argument);
            }
        }
        return Evaluate(function, parsedArguments);
    }

    Value Evaluate(const std::string function, const std::list<Value>& arguments = {}) const {
        return Evaluator::Evaluate(ast.Root(), function, arguments);
    }
};

} // namespace Interpreter