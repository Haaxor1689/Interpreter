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

    const std::map<std::string, Token::Type> operators = {
        std::make_pair("+", Token::Type::BinaryOperator),
        std::make_pair("-", Token::Type::BinaryOperator),
        std::make_pair("*", Token::Type::BinaryOperator),
        std::make_pair("/", Token::Type::BinaryOperator),
        std::make_pair("=", Token::Type::BinaryOperator),
        std::make_pair("<", Token::Type::BinaryOperator),
        std::make_pair(">", Token::Type::BinaryOperator),
        std::make_pair("!", Token::Type::UnaryOperator),
        std::make_pair("?", Token::Type::UnaryOperator),
        std::make_pair(".", Token::Type::BinaryOperator),
        std::make_pair(":", Token::Type::BinaryOperator),
        std::make_pair("|", Token::Type::BinaryOperator),
        std::make_pair("&", Token::Type::BinaryOperator),
        std::make_pair("++", Token::Type::UnaryOperator),
        std::make_pair("--", Token::Type::UnaryOperator),
        std::make_pair("+=", Token::Type::BinaryOperator),
        std::make_pair("-=", Token::Type::BinaryOperator),
        std::make_pair("->", Token::Type::BinaryOperator),
        std::make_pair("||", Token::Type::BinaryOperator),
        std::make_pair("&&", Token::Type::BinaryOperator),
        std::make_pair("==", Token::Type::BinaryOperator),
        std::make_pair("!=", Token::Type::BinaryOperator),
        std::make_pair("<=", Token::Type::BinaryOperator),
        std::make_pair(">=", Token::Type::BinaryOperator),
        std::make_pair("..", Token::Type::BinaryOperator),
        std::make_pair("...", Token::Type::BinaryOperator),
        std::make_pair("..>", Token::Type::BinaryOperator) };

public:
    Lexer(const std::filesystem::path& path) : source(path), current(source.get()) {}

    Token Next() {
        RemoveWhitespace();
        if (IsEoF(current))
            return Token("", Token::Type::EoF, line);

        if (IsIdentifierStart(current))
            return Identifier();

        if (IsNumberStart(current))
            return Number();

        if (IsStringMark(current))
            return String();
        
        if (IsOperator(current))
            return Operator();

        return Invalid();
    }

private:
    void RemoveWhitespace() {
        bool isComment = current == '#';
        while (std::isspace(current) || isComment) {
            if (current == '\n') {
                ++line;
                isComment = false;
            } else if (current == '#') {
                isComment = true;
            }
            current = source.get();
        }
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

    Token String() {
        do {
            if (IsEoF(current) || current == '\n') {
                return PopToken(Token::Type::Invalid);
            }
            buffer += current;
        } while (!IsStringMark(current = source.get()));
        buffer += current;
        current = source.get();
        return PopToken(Token::Type::String);
    }

    Token Operator() {
        do {
            buffer += current;
        } while (IsOperator(current = source.get()));

        if (auto it = operators.find(buffer); it != operators.end())
            return PopToken(it->second);
        return PopToken(Token::Type::Invalid);
    }

    Token Invalid() {
        buffer += current;
        current = source.get();
        return PopToken(Token::Type::Invalid);
    }

    bool IsEoF(char current) { return current == EOF; }
    bool IsIdentifierStart(char current) { return std::isalpha(current) || current == '_'; }
    bool IsIdentifier(char current) { return std::isalpha(current) || std::isdigit(current) || current == '_'; }
    bool IsNumberStart(char current) { return std::isdigit(current); }
    bool IsNumber(char current) { return std::isdigit(current) || current == '.'; }
    bool IsStringMark(char current) { return current == '\"'; }
    bool IsOperator(char current) { return operators.find(std::string(1, current)) != operators.end(); }
};
}
