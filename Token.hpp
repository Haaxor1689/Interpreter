#pragma once

#include <string>

namespace Interpreter {
struct Token {
    enum Type {
        Identifier, String, Number, UnaryOperator, BinaryOperator,
        ParenOpen, ParenClose,
        If, Elseif, Else, For, Do, While,
        EoF, Invalid
    };

    std::string text;
    Type type;
    unsigned line;

    Token(std::string text, Type type, unsigned line) : text(text), type(type), line(line) {}

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        return os << "[" << token.TypeString() << " \'" << token.text << "\' on line " << token.line << "]";
    }

    bool operator==(const Token& other) const {
        return text == other.text && type == other.type && line == other.line;
    }

    std::string TypeString() const {
        switch (type) {
            case Token::Type::Identifier: return "Identifier";
            case Token::Type::String: return "String";
            case Token::Type::Number: return "Number";
            case Token::Type::UnaryOperator: return "Unary Operator";
            case Token::Type::BinaryOperator: return "Binary Operator";
            case Token::Type::EoF: return "EoF";
            case Token::Type::Invalid: return "Invalid Token";
            default: return "Keyword";
        }
    }
};
}
