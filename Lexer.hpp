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
    const std::map<char, Token::Type> brackets = {
        std::make_pair('(', Token::Type::ParenOpen),
        std::make_pair(')', Token::Type::ParenClose),
        std::make_pair('[', Token::Type::SquareOpen),
        std::make_pair(']', Token::Type::SquareClose),
        std::make_pair('{', Token::Type::CurlyOpen),
        std::make_pair('}', Token::Type::CurlyClose) };

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

        if (auto it = brackets.find(current); it != brackets.end()) {
            current = source.get();
            return Token(std::string(1, it->first), it->second, line);
        }

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
        bool isDecimal = false;
        bool hasNumberAfterDecimal = false;
        bool isInvalid = false;
        do {
            if (current == '.') {
                if (isDecimal) isInvalid = true;
                isDecimal = true;
            } else if (isDecimal) {
                hasNumberAfterDecimal = true;
            }
            buffer += current;
        } while (IsNumber(current = source.get()));

        if (isInvalid || (isDecimal && !hasNumberAfterDecimal))
            return PopToken(Token::Type::Invalid);
        
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

    bool IsEoF(char c) { return c == EOF; }
    bool IsIdentifierStart(char c) { return std::isalpha(c) || c == '_'; }
    bool IsIdentifier(char c) { return std::isalpha(c) || std::isdigit(c) || c == '_'; }
    bool IsNumberStart(char c) { return std::isdigit(c); }
    bool IsNumber(char c) { return std::isdigit(c) || c == '.'; }
    bool IsStringMark(char c) { return c == '\"'; }
    bool IsOperator(char c) { return operators.find(std::string(1, c)) != operators.end(); }
};
}
