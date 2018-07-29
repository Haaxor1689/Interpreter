#pragma once

#include <variant>
#include <string>
#include <sstream>

namespace Interpreter {
    
// Helper class for variant visiting
template <class... Ts>
struct Visitor : Ts... { using Ts::operator()...; };
template <class... Ts>
Visitor(Ts...)->Visitor<Ts...>;

using VarID = unsigned;
using Value = std::variant<std::monostate, bool, double, std::string>;

using fVoidValuePtr = void(*)(const Value&);
using fStringPtr = std::string(*)();
using fDoublePtr = double(*)();
using ExtFunctionType = std::variant<std::monostate, fVoidValuePtr, fStringPtr, fDoublePtr>;

// Output operator for Value type
inline std::ostream& operator<<(std::ostream& os, const Value& val) {
    std::visit(
        Visitor{
            [&](const auto&) { os << "Void"; },
            [&](bool arg) { os << (arg ? "True" : "False"); },
            [&](double arg) { os << arg; },
            [&](const std::string& arg) { os << arg; },
        },
        val);
    return os;
}

// Output operator for Value type
inline std::string ToString(const Value& val) {
    using namespace std::string_literals;
    return std::visit(
        Visitor{
            [&](const auto&) { return "Void"s; },
            [&](bool arg) { return arg ? "True"s : "False"s; },
            [&](double arg) {
                std::ostringstream oss;
                oss << arg;
                return oss.str();
            },
            [&](const std::string& arg) { return arg; },
        },
        val);
}

}
