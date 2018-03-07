#pragma once

#include <filesystem>

#include "Lexer.hpp"
#include "Ast.hpp"

namespace Interpreter {
class Parser {
    Lexer lexer;
    Token token;

    Parser(const std::filesystem::path& path) : lexer(path), token(lexer.Next()) {}
    Ast operator()() {
        Ast ast;
        ast.root.MatchToken<0>(lexer.Next(), [this](){ return lexer.Next(); });
        // for (;token.type != Token::Type::EoF; token = lexer.Next()) {
            // std::cout << token << std::endl;
        // }
        return ast;
    }

public:
    static Ast Parse(const std::filesystem::path& path) {
        Parser parser(path);
        return parser();
    }
};
}