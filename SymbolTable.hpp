#pragma once

#include <map>
#include <string>
#include <algorithm>

#include "Exceptions.hpp"

namespace Interpreter {
    using VarID = unsigned;

struct Symbol {
    VarID id;
    VarID type;
    bool isFunction;
    bool isArray;
};

class SymbolTable {
    VarID nextID = 0;

    std::map<std::string, Symbol> local;
    SymbolTable* parent;

public:
    SymbolTable(SymbolTable* parent)
        : parent(parent) {}

    VarID AddSymbol(const std::string& key) {
        if (ContainsSymbol(key)) {
            throw IdentifierRedefinitionException(key);
        }
        return local[key].id = NextID();
    }

    const Symbol& GetSymbol(const std::string& key) const {
        auto it = local.find(key);
        if (it != local.end())
            return it->second;
        if (parent != nullptr)
            return parent->GetSymbol(key);
        throw UndefinedIdentifierException(key);
    }

    const Symbol& GetSymbol(VarID id) const {
        auto it = std::find_if(local.begin(), local.end(), [id](const auto& e) { return e.second.id == id; });
        if (it != local.end())
            return it->second;
        if (parent != nullptr)
            return parent->GetSymbol(id);
        throw UndefinedIdentifierNameException(id);
    }

    bool ContainsSymbol(const std::string& key) const {
        return local.find(key) != local.end() || (parent != nullptr && parent->ContainsSymbol(key));
    }

    std::string GetName(VarID id) const {
        auto it = std::find_if(local.begin(), local.end(), [id](const auto& e) { return e.second.id == id; });
        if (it != local.end())
            return std::to_string(id) + ":" + it->first;
        if (parent != nullptr)
            return parent->GetName(id);
        throw UndefinedIdentifierNameException(id);
    }

    void SetSymbol(VarID id, VarID type, bool isFunction, bool isArray) {
        auto it = std::find_if(local.begin(), local.end(), [id](const auto& e) { return e.second.id == id; });
        if (it != local.end()) {
            it->second.type = type;
            it->second.isFunction = isFunction;
            it->second.isArray = isArray;
            return;
        }
        if (parent != nullptr) {
            parent->SetSymbol(id, type, isFunction, isArray);
            return;
        }
        throw UndefinedIdentifierNameException(id);
    }

    VarID NextID() {
        if (parent != nullptr)
            return parent->NextID();
        return ++nextID;
    }

    friend std::ostream& operator<<(std::ostream& os, const SymbolTable& symbols) {
        os << "{ ";
        for (const auto& pair : symbols.local)
            os << pair.second.id << ":" << pair.first << ", ";
        return os << "}";
    }
};

} // namespace Interpreter