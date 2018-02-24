#pragma once

#include <fstream>
#include <map>
#include <regex>
#include <filesystem>

#include "Token.hpp"

namespace Interpreter {
class Lexer {
    std::ifstream source;
    std::string buffer;
    char current;
    unsigned line = 1;

public:
    Lexer(const std::filesystem::path& path) : source(path), current(source.get()) {}

    Token Next() {
        RemoveWhitespace();
        if (IsEoF(current))
            return Token("", Token::Type::EoF, line);

        if (IsIdentifier(current))
            return Identifier();

        if (IsNumber(current))
            return Number();

        if (IsSpecial(current))
            return Special();

        return Token(buffer, Token::Type::Invalid, line);
    }

private:
    void RemoveWhitespace() {
        bool isComment = false;
        do {
            if (current == '\n') {
                ++line;
                isComment = false;
            } else if (current == '#') {
                isComment = true;
            }
        } while (std::isspace(current = source.get()) || isComment);
    }

    Token PopToken(Token::Type type) {
        Token token(buffer, type, line);
        buffer.clear();
        return token;
    }

    Token Identifier() {
        do {
            buffer += current;
        } while (IsIdentifier(current = source.get()));
        return PopToken(Token::Type::Identifier);
    }

    Token Number() {
        do {
            buffer += current;
        } while (IsNumber(current = source.get()));
        return PopToken(Token::Type::Number);
    }

    Token Special() {
        return Token(buffer, Token::Type::Invalid, line);
    }

    bool IsEoF(char current) {
        return current == EOF;
    }

    bool IsIdentifier(char current) {
        return std::isalpha(current) || current == '_';
    }

    bool IsNumber(char current) {
        return std::isdigit(current);
    }

    bool IsSpecial(char current) {
        return std::ispunct(current);
    }
};
}