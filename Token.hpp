#pragma once

#include <string>

namespace Interpreter {
struct Token {
    enum Type {
        If, Else, Identifier, ParenOpen, ParenClose, String, Number, EoF, Invalid
    };

    std::string text;
    Type type;
    unsigned line;

    Token(std::string text, Type type, unsigned line) : text(text), type(type), line(line) {}

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        return os << token.text << "(" << token.TypeString() << ", line " << token.line << ")";
    }

    std::string TypeString() const {
        switch (type) {
            case Token::Type::String: return "String";
            case Token::Type::Number: return "Number";
            case Token::Type::EoF: return "EoF";
            case Token::Type::Invalid: return "Invalid Token";
            default: return "Other";
        }
    }
};
}