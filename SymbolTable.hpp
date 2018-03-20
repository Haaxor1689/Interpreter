#pragma once

#include <map>
#include <string>

namespace Interpreter {

struct SymbolTable {
    using VarID = unsigned;
    std::map<std::string, VarID> local;
    SymbolTable* parent;

public:
    SymbolTable(SymbolTable* parent)
        : parent(parent) {}

    VarID GetSymbol(const std::string& key) const {
        VarID id = local.at(key);
        if (id == 0 && parent != nullptr)
            id = parent->GetSymbol(key);
        return id;
    }

    unsigned AddSymbol(const std::string& key) {
        if (ContainsSymbol(key))
            return 0;
        return local[key] = NextID();
    }

    bool ContainsSymbol(const std::string& key) const {
        return local.find(key) != local.end() || (parent != nullptr && parent->ContainsSymbol(key));
    }

    static VarID NextID() {
        static unsigned id = 0;
        return ++id;
    }
};

} // namespace Interpreter