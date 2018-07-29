#pragma once

#include <fstream>
#include <map>
#include <regex>
#include <cctype>

#include "Token.hpp"

namespace Interpreter {

class Lexer {
    std::istream& source;
    std::string buffer;
    char current;
    unsigned line = 1;

    const std::map<std::string, Token::Type> operators = {
        std::make_pair("+", Token::Type::BinaryOperator), std::make_pair("-", Token::Type::BinaryOperator),
        std::make_pair("*", Token::Type::BinaryOperator), std::make_pair("/", Token::Type::BinaryOperator),
        std::make_pair("=", Token::Type::BinaryOperator), std::make_pair("<", Token::Type::BinaryOperator),
        std::make_pair(">", Token::Type::BinaryOperator), std::make_pair("!", Token::Type::UnaryOperator),
        std::make_pair("?", Token::Type::UnaryOperator), std::make_pair(".", Token::Type::BinaryOperator),
        std::make_pair("|", Token::Type::BinaryOperator), std::make_pair("&", Token::Type::BinaryOperator),
        std::make_pair("++", Token::Type::UnaryOperator), std::make_pair("--", Token::Type::UnaryOperator),
        std::make_pair("+=", Token::Type::BinaryOperator), std::make_pair("-=", Token::Type::BinaryOperator),
        std::make_pair("->", Token::Type::BinaryOperator), std::make_pair("||", Token::Type::BinaryOperator),
        std::make_pair("&&", Token::Type::BinaryOperator), std::make_pair("==", Token::Type::BinaryOperator),
        std::make_pair("!=", Token::Type::BinaryOperator), std::make_pair("<=", Token::Type::BinaryOperator),
        std::make_pair(">=", Token::Type::BinaryOperator), std::make_pair("..", Token::Type::BinaryOperator),
        std::make_pair("...", Token::Type::RangeOperator), std::make_pair("..<", Token::Type::RangeOperator)
    };
    const std::map<char, Token::Type> special = {
        std::make_pair('(', Token::Type::ParenOpen), std::make_pair(')', Token::Type::ParenClose),
        std::make_pair('[', Token::Type::SquareOpen), std::make_pair(']', Token::Type::SquareClose),
        std::make_pair('{', Token::Type::CurlyOpen), std::make_pair('}', Token::Type::CurlyClose),
        std::make_pair(':', Token::Type::Colon), std::make_pair(';', Token::Type::Semicolon),
        std::make_pair(',', Token::Type::Comma)
    };
    const std::map<std::string, Token::Type> keywords = {
        std::make_pair("if", Token::Type::If), std::make_pair("elseif", Token::Type::Elseif),
        std::make_pair("else", Token::Type::Else), std::make_pair("for", Token::Type::For),
        std::make_pair("do", Token::Type::Do), std::make_pair("while", Token::Type::While),
        std::make_pair("return", Token::Type::Return), std::make_pair("func", Token::Type::Func),
        std::make_pair("in", Token::Type::In), std::make_pair("as", Token::Type::As),
        std::make_pair("null", Token::Type::Null), std::make_pair("var", Token::Type::Var),
        std::make_pair("true", Token::Type::True), std::make_pair("false", Token::Type::False),
    };

public:
    Lexer(std::istream& stream)
        : source(stream), current(char(source.get())) {}

    Token Next() {
        RemoveWhitespace();
        if (IsEoF(current))
            return Token("", Token::Type::EoF, line);

        if (IsIdentifierStart(current))
            return Identifier();

        if (IsMinus(current))
            return Minus();

        if (IsNumberStart(current))
            return Number();

        if (IsStringMark(current))
            return String();

        if (IsOperator(current))
            return Operator();

        if (auto it = special.find(current); it != special.end()) {
            current = char(source.get());
            return Token(std::string(1, it->first), it->second, line);
        }

        return Invalid();
    }

private:
    void RemoveWhitespace() {
        bool isComment = current == '#';
        while (std::isspace(current) || current == '#' || isComment) {
            if (current == '\n') {
                ++line;
                isComment = false;
            } else if (current == '#') {
                isComment = true;
            }
            current = char(source.get());
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
        } while (IsIdentifier(current = char(source.get())));

        if (auto it = keywords.find(buffer); it != keywords.end())
            return PopToken(it->second);

        return PopToken(Token::Type::Identifier);
    }

    Token Minus() {
        buffer += '-';
        current = char(source.get());
        if (IsNumber(current)) {
            return Number();
        } else if (IsOperator(current)) {
            return Operator();
        }
        return PopToken(Token::Type::BinaryOperator);
    }

    Token Number() {
        bool isDecimal = false;
        bool hasNumberAfterDecimal = false;
        bool isInvalid = false;
        do {
            if (current == '.') {
                if (isDecimal)
                    isInvalid = true;
                isDecimal = true;
            } else if (isDecimal) {
                hasNumberAfterDecimal = true;
            }
            buffer += current;
        } while (IsNumber(current = char(source.get())));

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
        } while (!IsStringMark(current = char(source.get())));
        buffer += current;
        current = char(source.get());
        return PopToken(Token::Type::String);
    }

    Token Operator() {
        do {
            buffer += current;
        } while (IsOperator(current = char(source.get())));

        if (auto it = operators.find(buffer); it != operators.end())
            return PopToken(it->second);
        return PopToken(Token::Type::Invalid);
    }

    Token Invalid() {
        buffer += current;
        current = char(source.get());
        return PopToken(Token::Type::Invalid);
    }

    bool IsEoF(char c) {
        return c == EOF;
    }
    bool IsIdentifierStart(char c) {
        return std::isalpha(c) || c == '_';
    }
    bool IsIdentifier(char c) {
        return std::isalpha(c) || std::isdigit(c) || c == '_';
    }
    bool IsMinus(char c) {
        return c == '-';
    }
    bool IsNumberStart(char c) {
        return std::isdigit(c);
    }
    bool IsNumber(char c) {
        return std::isdigit(c) || c == '.';
    }
    bool IsStringMark(char c) {
        return c == '\"';
    }
    bool IsOperator(char c) {
        return operators.find(std::string(1, c)) != operators.end();
    }
};

} // namespace Interpreter
