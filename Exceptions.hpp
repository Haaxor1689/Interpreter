#pragma once

#include <exception>
#include <sstream>
#include <vector>

#include "Token.hpp"

namespace Interpreter {

struct ParseException : public std::exception {
    ParseException(Token received, const std::vector<Token::Type>& expected)
        : received(received), expected(expected) {}

    Token received;
    std::vector<Token::Type> expected;

    virtual char const* what() const throw() {
        std::stringstream ret;
        ret << "Failed to parse " << received << ". Expected ";
        if (expected.size() == 0) {
            ret << Token::TypeString(Token::Type::Invalid);
        } else if (expected.size() == 1) {
            ret << Token::TypeString(expected[0]);
        } else {
            ret << "one of following { ";
            for (auto tokenType : expected) {
                ret << Token::TypeString(tokenType) + ", ";
            }
            ret << "}";
        }
        ret << ".";
        return ret.str().c_str();
    }
};

struct UndefinedIdentifierException : public std::exception {
    UndefinedIdentifierException(const std::string& identifier)
        : identifier(identifier) {}

    std::string identifier;

    virtual char const* what() const throw() {
        std::stringstream ret;
        ret << "Found undefined identifier " << identifier << ".";
        return ret.str().c_str();
    }
};

struct UndefinedIdentifierNameException : public std::exception {
    UndefinedIdentifierNameException(unsigned identifier)
        : identifier(identifier) {}

    unsigned identifier;

    virtual char const* what() const throw() {
        std::stringstream ret;
        ret << "Tried to access undefined identifier with id " << identifier << ".";
        return ret.str().c_str();
    }
};

struct IdentifierRedefinitionException : public std::exception {
    IdentifierRedefinitionException(const std::string& identifier)
        : identifier(identifier) {}

    std::string identifier;

    virtual char const* what() const throw() {
        std::stringstream ret;
        ret << "Tried to redefine identifier " << identifier << ".";
        return ret.str().c_str();
    }
};

} // namespace Interpreter