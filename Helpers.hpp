#pragma once

#include <map>
#include <sstream>
#include <string>
#include <variant>

#include "Exceptions.hpp"

namespace Interpreter {
    
// Helper class for variant visiting
template <class... Ts>
struct Visitor : Ts... { using Ts::operator()...; };
template <class... Ts>
Visitor(Ts...)->Visitor<Ts...>;

struct Object;
struct Array;
using VarID = unsigned;
using VarRef = std::variant<VarID, std::string>;
using Value = std::variant<std::monostate, bool, double, std::string, Object, Array>;

struct Object {
    Object(std::map<std::string, Value>&& values) : values(std::move(values)) {}

    friend bool operator==(const Object&, const Object&) { throw InternalException("NotImplemented"); }
    friend bool operator!=(const Object&, const Object&) { throw InternalException("NotImplemented"); }
    friend bool operator>=(const Object&, const Object&) { throw InternalException("NotImplemented"); }
    friend bool operator<=(const Object&, const Object&) { throw InternalException("NotImplemented"); }
    friend bool operator>(const Object&, const Object&) { throw InternalException("NotImplemented"); }
    friend bool operator<(const Object&, const Object&) { throw InternalException("NotImplemented"); }

    std::map<std::string, Value> values;
};

struct Array {
    Array(std::vector<Value>&& values) : values(std::move(values)) {}

    friend bool operator==(const Array&, const Array&) { throw InternalException("NotImplemented"); }
    friend bool operator!=(const Array&, const Array&) { throw InternalException("NotImplemented"); }
    friend bool operator>=(const Array&, const Array&) { throw InternalException("NotImplemented"); }
    friend bool operator<=(const Array&, const Array&) { throw InternalException("NotImplemented"); }
    friend bool operator>(const Array&, const Array&) { throw InternalException("NotImplemented"); }
    friend bool operator<(const Array&, const Array&) { throw InternalException("NotImplemented"); }

    std::vector<Value> values;
};

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
            [&](const std::string& arg) { os << "\"" << arg << "\""; },
            [&](const Object& arg) {
                os << "{ ";
                for (const auto& value : arg.values) {
                    os << value.first << ": " << value.second << ", ";
                }
                os << "}";
            },
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
            [&](const Object& arg) {
                std::ostringstream oss;
                oss << arg;
                return oss.str();
            },
        },
        val);
}

}
