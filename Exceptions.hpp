#pragma once

#include <exception>
#include <sstream>
#include <vector>

#include "Token.hpp"

namespace Interpreter {

struct ParseException : public std::exception {
    ParseException(Token received, const std::vector<Token::Type>& expected) {
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
    }

    char const* what() const noexcept override {
        return ret.str().c_str();
    }

private:
    std::stringstream ret;
};

struct UndefinedIdentifierException : public std::exception {
    UndefinedIdentifierException(const std::string& identifier)
        : message("Found undefined identifier " + identifier + ".") {}

    char const* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

struct UndefinedIdentifierNameException : public std::exception {
    UndefinedIdentifierNameException(unsigned identifier)
        : message("Tried to access undefined identifier with id " + std::to_string(identifier) + ".") {}

    char const* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

struct IdentifierRedefinitionException : public std::exception {
    IdentifierRedefinitionException(const std::string& identifier)
        : message("Tried to redefine identifier " + identifier + ".") {}

    char const* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

} // namespace Interpreter