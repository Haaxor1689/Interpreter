#pragma once

#include <map>
#include <variant>
#include <algorithm>

#include "Ast.hpp"

namespace Interpreter {

class Evaluator {
    std::map<VarID, Value> localValues;
    Evaluator* parent;
    const Global& root;

    Evaluator(Evaluator* parent, const Global& root)
        : parent(parent), root(root) {}

public:
    static Value Evaluate(const Global& root, const std::string& function, const std::list<Value>& arguments) {
        const FunctionDef& func = root.GetFunction(function);
        if (arguments.size() != func.arguments->arguments.size()) {
            throw std::runtime_error("Wrong number of arguments.");
        }

        Evaluator eval(nullptr, root);
        auto inIt = arguments.begin();
        auto argIt = func.arguments->arguments.begin();
        for (; inIt != arguments.end(); ++inIt, ++argIt) {
            eval.localValues.emplace(argIt->name, *inIt);
        }

        return eval.Evaluate(*func.block);
    }

private:
    Value GetValue(VarID variable) {
        auto ptr = this;
        while (ptr) {
            auto it = ptr->localValues.find(variable);
            if (it != ptr->localValues.end()) {
                return it->second;
            }
            ptr = ptr->parent;
        }
        throw std::runtime_error("Variable with id " + std::to_string(variable) + " not found.");
    }

    Value Evaluate(const Block& node) {
        bool didHitReturn = false;
        for (const Statement& statement : node.statements) {
            Value value = std::visit(
                Visitor{
                    [&](const auto&) { return Value(); },
                    [&](const Return& arg) { didHitReturn = true; return Evaluate(*arg.value); },
                    [&](const ForExpr& arg) { Evaluator(this, root).Evaluate(arg); return Value(); },
                    [&](const IfExpr& arg) { Evaluate(arg); return Value(); },
                    [&](const WhileExpr& arg) { Evaluator(this, root).Evaluate(arg); return Value(); },
                    [&](const Expression& arg) { Evaluate(arg); return Value(); },
                },
                statement.expression);
            if (didHitReturn)
                return value;
        }
        return Value();
    }

    void Evaluate(const ForExpr&) {
    }

    void Evaluate(const IfExpr& node) {
        auto ifEval = Evaluator(this, root);
        if (std::get<bool>(ifEval.Evaluate(*node.ifStatement->condition))) {
            ifEval.Evaluate(*node.ifStatement->block);
        }

        for (const auto& elsif : node.elseifStatements) {
            auto elseifEval = Evaluator(this, root);
            if (std::get<bool>(elseifEval.Evaluate(*elsif.condition))) {
                elseifEval.Evaluate(*node.ifStatement->block);
                return;
            }
        }

        if (node.elseStatement) {
            Evaluator(this, root).Evaluate(*node.elseStatement->block);
        }
    }

    void Evaluate(const WhileExpr&) {
    }

    Value Evaluate(const Expression& node) {
        return std::visit(
            Visitor{
                [&](const auto&) { return Value(); },
                [&](const VariableRef& arg) { return GetValue(arg.name); },
                [&](const FunctionCall& arg) { return Evaluate(arg); },
                [&](bool arg) { return Value(arg); },
                [&](double arg) { return Value(arg); },
                [&](const std::string& arg) { return Value(arg); },
                [&](const VariableDef& arg) { return Evaluate(arg); },
            },
            node.expression);
    }

    Value Evaluate(const FunctionCall& node) {
        const FunctionDef& func = root.GetFunction(node.name);
        if (node.arguments.size() != func.arguments->arguments.size()) {
            throw std::runtime_error("Wrong number of arguments.");
        }

        Evaluator eval(this, root);
        auto inIt = node.arguments.begin();
        auto argIt = func.arguments->arguments.begin();
        for (; inIt != node.arguments.end(); ++inIt, ++argIt) {
            eval.localValues.emplace(argIt->name, Evaluate(*inIt));
        }

        return eval.Evaluate(*func.block.get());
    }

    Value Evaluate(const VariableDef& node) {
        localValues.emplace(node.name, Value());
        return GetValue(node.name);
    }
};

} // namespace Interpreter