#pragma once

#include <map>
#include <string>
#include <algorithm>

#include "Exceptions.hpp"

namespace Interpreter {

class SymbolTable {
    using VarID = unsigned;
    std::map<std::string, VarID> local;
    SymbolTable* parent;

public:
    SymbolTable(SymbolTable* parent)
        : parent(parent) {}

    VarID AddSymbol(const std::string& key) {
        if (ContainsSymbol(key))
            throw IdentifierRedefinitionException(key);
        return local[key] = NextID();
    }

    VarID GetSymbol(const std::string& key) const {
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
        auto it = std::find_if(local.begin(), local.end(), [id](const auto& e) { return e.second == id; });
        if (it != local.end())
            return it->first;
        if (parent != nullptr)
            return parent->GetName(id);
        return ""; // TODO - throw custom exception
    }

    static VarID NextID() {
        static VarID id = 0;
        return ++id;
    }

    friend std::ostream& operator<<(std::ostream& os, const SymbolTable& symbols) {
        os << "{ ";
        for (const auto& pair : symbols.local)
            os << pair.second << ":" << pair.first << ", ";
        return os << "}";
    }
};

} // namespace Interpreter