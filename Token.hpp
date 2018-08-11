#pragma once

#include <string>

namespace Interpreter {

struct Token {
    enum Type {
        Identifier,
        String,
        Number,
        UnaryOperator,
        BinaryOperator,
        RangeOperator,
        ParenOpen,
        ParenClose,
        SquareOpen,
        SquareClose,
        CurlyOpen,
        CurlyClose,
        If,
        Elseif,
        Else,
        For,
        Do,
        While,
        Return,
        Func,
        In,
        As,
        Null,
        Var,
        True,
        False,
        Colon,
        Semicolon,
        Comma,
        Dot,
        Object,
        New,
        EoF,
        Invalid
    };

    std::string text;
    Type type;
    unsigned line;

    Token(std::string text, Type type, unsigned line)
        : text(text), type(type), line(line) {}

    friend std::ostream& operator<<(std::ostream& os, const Token& token) {
        return os << "[" << token.TypeString() << " \'" << token.text << "\' on line " << token.line << "]";
    }

    bool operator==(const Token& other) const {
        return text == other.text && type == other.type && line == other.line;
    }

    std::string TypeString() const {
        return TypeString(type);
    }

    bool IsLogicalOperator() const {
        return text == "==" || text == "!=" || text == ">" || text == ">=" || text == "<" || text == "<=" || text == "&&" || text == "||" || text == "!";
    }

    bool ShouldIncludeLast() const {
        return text == "...";
    }

    static std::string TypeString(Type type) {
        switch (type) {
        case Identifier:
            return "Identifier";
        case String:
            return "String";
        case Number:
            return "Number";
        case UnaryOperator:
            return "Unary Operator";
        case BinaryOperator:
            return "Binary Operator";
        case RangeOperator:
            return "Range Operator";
        case ParenOpen:
        case ParenClose:
        case SquareOpen:
        case SquareClose:
        case CurlyOpen:
        case CurlyClose:
            return "Bracket";
        case If:
            return "If";
        case Elseif:
            return "Elseif";
        case Else:
            return "Else";
        case For:
            return "For";
        case Do:
            return "Do";
        case While:
            return "While";
        case Return:
            return "Return";
        case Func:
            return "Func";
        case In:
            return "In";
        case As:
            return "As";
        case Null:
            return "Null";
        case Var:
            return "Var";
        case True:
            return "True";
        case False:
            return "False";
        case Colon:
            return "Colon";
        case Semicolon:
            return "Semicolon";
        case Comma:
            return "Comma";
        case Dot:
            return "Dot";
        case Object:
            return "Object";
        case New:
            return "New";
        case EoF:
            return "EoF";
        case Invalid:
            return "Invalid Token";
        default:
            return "Keyword";
        }
    }
};

} // namespace Interpreter
