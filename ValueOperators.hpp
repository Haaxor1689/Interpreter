#pragma once

#include "Ast.hpp"

namespace Interpreter {

#define arithmeticOperators(name) \
inline Value operator##name(const Value& lhs, double rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw std::runtime_error("No operator for this type."); },\
            [&](double arg) { return Value(arg ##name rhs); },\
        },\
        lhs);\
}\
\
inline Value operator##name(const Value& lhs, const std::string& rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw std::runtime_error("No operator for this type."); },\
            [&](const std::string& arg) { return Value(arg ##name rhs); },\
        },\
        lhs);\
}\
\
inline Value operator##name(const Value& lhs, const Value& rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw std::runtime_error("No operator for this type."); },\
            [&](double arg) { return lhs ##name arg; },\
            [&](const std::string& arg) { return lhs ##name arg; },\
        },\
        rhs);\
}

#define logicalOperators(name) \
inline Value operator##name(const Value& lhs, bool rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw std::runtime_error("No operator for this type."); },\
            [&](bool arg) { return Value(arg ##name rhs); },\
        },\
        lhs);\
}\
\
inline Value operator##name(const Value& lhs, double rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw std::runtime_error("No operator for this type."); },\
            [&](double arg) { return Value(arg ##name rhs); },\
        },\
        lhs);\
}\
\
inline Value operator##name(const Value& lhs, const std::string& rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw std::runtime_error("No operator for this type."); },\
            [&](const std::string& arg) { return Value(arg ##name rhs); },\
        },\
        lhs);\
}\
\
inline Value operator##name(const Value& lhs, const Value& rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw std::runtime_error("No operator for this type."); },\
            [&](bool arg) { return lhs ##name arg; },\
            [&](double arg) { return lhs ##name arg; },\
            [&](const std::string& arg) { return lhs ##name arg; },\
        },\
        rhs);\
}
arithmeticOperators(+);
arithmeticOperators(-);
arithmeticOperators(*);
arithmeticOperators(/);
logicalOperators(&&);
logicalOperators(||);

} // namespace Interpreter
