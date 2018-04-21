#pragma once

#include "Ast.hpp"

namespace Interpreter {

inline Value operator+(const Value& lhs, double rhs) {
    return std::visit(
        Visitor{
            [&](const auto&) -> Value { throw std::runtime_error("No operator+ for this type."); },
            [&](double arg) { return Value(rhs + arg); },
        },
        lhs);
}

inline Value operator+(const Value& lhs, const std::string& rhs) {
    return std::visit(
        Visitor{
            [&](const auto&) -> Value { throw std::runtime_error("No operator+ for this type."); },
            [&](const std::string& arg) { return Value(rhs + arg); },
        },
        lhs);
}

inline Value operator+(const Value& lhs, const Value& rhs) {
    return std::visit(
        Visitor{
            [&](const auto&) -> Value { throw std::runtime_error("No operator+ for this type."); },
            [&](double arg) { return lhs + arg; },
            [&](const std::string& arg) { return lhs + arg; },
        },
        rhs);
}

} // namespace Interpreter