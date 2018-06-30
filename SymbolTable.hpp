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
};

class SymbolTable {
    VarID nextID = 0;

    std::map<std::string, Symbol> local;
    SymbolTable* parent;

public:
    SymbolTable(SymbolTable* parent)
        : parent(parent) {}

    VarID AddSymbol(const std::string& key, VarID type = 0) {
        if (ContainsSymbol(key))
            throw IdentifierRedefinitionException(key);

        local[key].type = type;
        local[key].id = NextID();
        return local[key].id;
    }

    Symbol GetSymbol(const std::string& key) const {
        auto it = local.find(key);
        if (it != local.end())
            return it->second;
        if (parent != nullptr)
            return parent->GetSymbol(key);
        throw UndefinedIdentifierException(key);
    }

    bool ContainsSymbol(const std::string& key) const {
        return local.find(key) != local.end() || (parent != nullptr && parent->ContainsSymbol(key));
    }

    std::string GetName(VarID id) const {
        auto it = std::find_if(local.begin(), local.end(), [id](const auto& e) { return e.second.id == id; });
        if (it != local.end())
            return it->first;
        if (parent != nullptr)
            return parent->GetName(id);
        throw UndefinedIdentifierNameException(id);
    }

    VarID GetType(VarID id) const {
        auto it = std::find_if(local.begin(), local.end(), [id](const auto& e) { return e.second.id == id; });
        if (it != local.end())
            return it->second.type;
        if (parent != nullptr)
            return parent->GetType(id);
        throw UndefinedIdentifierNameException(id);
    }

    void SetType(VarID id, VarID type) {
        auto it = std::find_if(local.begin(), local.end(), [id](const auto& e) { return e.second.id == id; });
        if (it != local.end()) {
            it->second.type = type;
            return;
        }
        if (parent != nullptr) {
            parent->SetType(id, type);
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