#pragma once

#include <string>

namespace Interpreter {
struct Token {
    enum Type {
        Identifier, String, Number, UnaryOperator, BinaryOperator,
        ParenOpen, ParenClose, SquareOpen, SquareClose, CurlyOpen, CurlyClose,
        If, Elseif, Else, For, Do, While, Return, Func, In, As,
        Null, Var, True, False,
        Colon, Semicolon, Comma,
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
            case Token::Type::ParenOpen:
            case Token::Type::ParenClose:
            case Token::Type::SquareOpen:
            case Token::Type::SquareClose:
            case Token::Type::CurlyOpen:
            case Token::Type::CurlyClose: return "Bracket";
            case Token::Type::Colon:
            case Token::Type::Semicolon:
            case Token::Type::Comma: return "Special Token";
            case Token::Type::EoF: return "EoF";
            case Token::Type::Invalid: return "Invalid Token";
            default: return "Keyword";
        }
    }
};
}
