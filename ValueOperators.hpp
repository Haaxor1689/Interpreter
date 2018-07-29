#pragma once

#include "Ast.hpp"

namespace Interpreter {

#define arithmeticOperators(name) \
inline Value operator##name(const Value& lhs, double rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw InternalException("Invalid operator use."); },\
            [&](double arg) { return Value(arg ##name rhs); },\
        },\
        lhs);\
}\
\
inline Value operator##name(const Value& lhs, const std::string& rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw InternalException("Invalid operator use."); },\
            [&](const std::string& arg) { return Value(arg ##name rhs); },\
        },\
        lhs);\
}\
\
inline Value operator##name(const Value& lhs, const Value& rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw InternalException("Invalid operator use."); },\
            [&](double arg) { return lhs ##name arg; },\
            [&](const std::string& arg) { return lhs ##name arg; },\
        },\
        rhs);\
}

#define logicalOperators(name) \
inline Value operator##name(const Value& lhs, bool rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw InternalException("Invalid operator use."); },\
            [&](bool arg) { return Value(arg ##name rhs); },\
        },\
        lhs);\
}\
\
inline Value operator##name(const Value& lhs, double rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw InternalException("Invalid operator use."); },\
            [&](double arg) { return Value(arg ##name rhs); },\
        },\
        lhs);\
}\
\
inline Value operator##name(const Value& lhs, const std::string& rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw InternalException("Invalid operator use."); },\
            [&](const std::string& arg) { return Value(arg ##name rhs); },\
        },\
        lhs);\
}\
\
inline Value operator##name(const Value& lhs, const Value& rhs) {\
    return std::visit(\
        Visitor{\
            [&](const auto&) -> Value { throw InternalException("Invalid operator use."); },\
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

inline Value operator!(const Value& value) {
return std::visit(
    Visitor{
        [&](const auto&) -> Value { throw InternalException("Invalid operator use."); },
        [&](bool arg) { return Value(!arg); },
    },
    value);
}

inline Value operator++(const Value& value) {
return std::visit(
    Visitor{
        [&](const auto&) -> Value { throw InternalException("Invalid operator use."); },
        [&](double arg) { return Value(++arg); },
    },
    value);
}

inline Value operator--(const Value& value) {
return std::visit(
    Visitor{
        [&](const auto&) -> Value { throw InternalException("Invalid operator use."); },
        [&](double arg) { return Value(--arg); },
    },
    value);
}

} // namespace Interpreter
