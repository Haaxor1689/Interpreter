#pragma once

#include <exception>
#include <sstream>
#include <vector>

#include "Token.hpp"

namespace Interpreter {
struct ParseError: public std::exception {
    ParseError(Token received, const std::vector<Token::Type>& expected) : received(received), expected(expected) {}

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
}