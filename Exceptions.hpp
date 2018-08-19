#pragma once

#include <exception>
#include <sstream>
#include <vector>

#include "Token.hpp"

namespace Interpreter {

struct InternalException : public std::exception {
    InternalException(const std::string& message) : message(message) {}
    char const* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

struct InterpreterException : public std::exception {
    InterpreterException(const std::string& message, unsigned line) : message("An exception occured on line " + std::to_string(line) + ". Message: " + message) {}
    char const* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

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
    
    UndefinedIdentifierException(unsigned identifier)
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
    TypeMismatchException(const std::string& expected, const std::string& actual, unsigned line, const std::string& cause = "")
        : message("Type mismatch error on line " + std::to_string(line) + (cause.empty() ? "" : " caused by " + cause) + ". Expected \"" + expected + "\" got \"" + actual + "\".") {}

    char const* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

} // namespace Interpreter