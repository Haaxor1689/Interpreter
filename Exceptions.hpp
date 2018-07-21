#pragma once

#include <exception>
#include <sstream>
#include <vector>

#include "Token.hpp"

namespace Interpreter {

struct ParseException : public std::exception {
    ParseException(Token received, const std::vector<Token::Type>& expected) {
        std::stringstream sstream;
        sstream << "Failed to parse " << received << ". Expected ";
        if (expected.empty()) {
            sstream << Token::TypeString(Token::Type::Invalid);
        } else if (expected.size() == 1) {
            sstream << Token::TypeString(expected[0]);
        } else {
            sstream << "one of following { ";
            for (auto tokenType : expected) {
                sstream << Token::TypeString(tokenType) + ", ";
            }
            sstream << "}";
        }
        sstream << ".";
        message = sstream.str();
    }

    char const* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
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

struct TypeMismatchException : public std::exception {
    TypeMismatchException(const std::string& expected, const std::string& actual, unsigned line)
        : message("Type mismatch error on line " + std::to_string(line) + ". Expected \"" + expected + "\" got \"" + actual + "\".") {}

    char const* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

} // namespace Interpreter