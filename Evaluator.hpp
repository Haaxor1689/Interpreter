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
    Value Evaluate(const Block& node) {
        bool didHitReturn = false;
        for (const Statement& statement : node.statements) {
            Value value = std::visit(
                Visitor{
                    [&](const auto&) { return Value(); },
                    [&](const Return& arg) { didHitReturn = true; return Evaluator(this, root).Evaluate(*arg.value); },
                    [&](const ForExpr& arg) { Evaluator(this, root).Evaluate(arg); return Value(); },
                    [&](const IfExpr& arg) { Evaluator(this, root).Evaluate(arg); return Value(); },
                    [&](const WhileExpr& arg) { Evaluator(this, root).Evaluate(arg); return Value(); },
                    [&](const Expression& arg) { Evaluator(this, root).Evaluate(arg); return Value(); },
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
        if (std::get<bool>(Evaluator(this, root).Evaluate(*node.ifStatement->condition))) {
            Evaluator(this, root).Evaluate(*node.ifStatement->block);
        }

        for (const auto& elsif : node.elseifStatements) {
            if (std::get<bool>(Evaluator(this, root).Evaluate(*elsif.condition))) {
                Evaluator(this, root).Evaluate(*node.ifStatement->block);
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
                [&](const VariableRef& arg) { return localValues[arg.name]; },
                [&](const FunctionCall& arg) { return Evaluator(this, root).Evaluate(arg); },
                [&](bool arg) { return Value(arg); },
                [&](double arg) { return Value(arg); },
                [&](const std::string& arg) { return Value(arg); },
                [&](const VariableDef& arg) { return Evaluator(this, root).Evaluate(arg); },
            },
            node.expression);
    }

    Value Evaluate(const FunctionCall& node) {
        const FunctionDef& func = root.GetFunction(node.name);
        if (node.arguments.size() != func.arguments->arguments.size()) {
            throw std::runtime_error("Wrong number of arguments.");
        }

        Evaluator eval(nullptr, root);
        auto inIt = node.arguments.begin();
        auto argIt = func.arguments->arguments.begin();
        for (; inIt != node.arguments.end(); ++inIt, ++argIt) {
            eval.localValues.emplace(argIt->name, Evaluator(this, root).Evaluate(*inIt));
        }

        return eval.Evaluate(*func.block.get());
    }

    Value Evaluate(const VariableDef& node) {
        localValues.emplace(node.name, Value());
        return localValues[node.name];
    }
};

} // namespace Interpreter