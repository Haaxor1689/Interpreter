#pragma once

#include <map>
#include <string>
#include <algorithm>

#include "Exceptions.hpp"

namespace Interpreter {
    using VarID = unsigned;

struct Symbol {
    VarID id;
    std::string name;
    VarID type;
    bool isFunction;
    bool isArray;

    friend std::ostream& operator<<(std::ostream& os, const Symbol& symbol) {
        return os << symbol.id << ":" << symbol.name;
    }

    friend std::string ToString(const Symbol& symbol) {
        return std::to_string(symbol.id) + ":" + symbol.name;
    }
};

class SymbolTable {
    VarID nextID = 0;

    std::map<std::string, Symbol> local;
    SymbolTable* parent;

public:
    SymbolTable(SymbolTable* parent)
        : parent(parent) {}

    VarID Add(const std::string& key) {
        if (Contains(key)) {
            throw IdentifierRedefinitionException(key);
        }
        local[key].name = key;
        return local[key].id = NextID();
    }

    bool Contains(const std::string& key) const {
        return local.find(key) != local.end() || (parent != nullptr && parent->Contains(key));
    }

    const Symbol& operator[](const VarRef& ref) const {
        auto it = std::visit(
            Visitor{
                [&](VarID arg) { return std::find_if(local.begin(), local.end(), [arg](const auto& e) { return e.second.id == arg; }); },
                [&](const std::string& arg) { return local.find(arg); },
            },
            ref
        );
        if (it != local.end())
            return it->second;
        if (parent != nullptr)
            return (*parent)[ref];

        std::visit(
            Visitor{
                [ref](VarID arg) { throw UndefinedIdentifierException(arg); },
                [ref](const std::string& arg) { throw UndefinedIdentifierException(arg); },
            },
            ref
        );
    }

    void Set(VarID id, VarID type, bool isFunction, bool isArray) {
        auto it = std::find_if(local.begin(), local.end(), [id](const auto& e) { return e.second.id == id; });
        if (it != local.end()) {
            it->second.type = type;
            it->second.isFunction = isFunction;
            it->second.isArray = isArray;
            return;
        }
        if (parent != nullptr) {
            parent->Set(id, type, isFunction, isArray);
            return;
        }
        throw UndefinedIdentifierException(id);
    }

    VarID NextID() {
        if (parent != nullptr)
            return parent->NextID();
        return ++nextID;
    }

    friend std::ostream& operator<<(std::ostream& os, const SymbolTable& symbols) {
        os << "[ ";
        for (const auto& pair : symbols.local)
            os << "\"" << pair.second << "\", ";
        return os << "]";
    }
};

} // namespace Interpreter