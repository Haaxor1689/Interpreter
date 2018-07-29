#pragma once

#include <map>
#include <variant>
#include <algorithm>

#include "Ast.hpp"
#include "ValueOperators.hpp"

namespace Interpreter {

class Evaluator {
    std::map<VarID, Value> localValues;
    Evaluator* parent;
    const Global& root;
    bool didHitReturn = false;

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
    Value& GetValue(VarID variable) {
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
        for (const Statement& statement : node.statements) {
            Value value = std::visit(
                Visitor{
                    [&](const auto&) -> Value { throw; },
                    [&](const Return& arg) { 
                        didHitReturn = true; 
                        return Evaluate(*arg.value);
                    },
                    [&](const ForExpr& arg) {
                        auto eval = Evaluator(this, root);
                        auto value = eval.Evaluate(arg);
                        didHitReturn = eval.didHitReturn;
                        return value;
                    },
                    [&](const IfExpr& arg) { return Evaluate(arg); },
                    [&](const WhileExpr& arg) {
                        auto eval = Evaluator(this, root);
                        auto value = eval.Evaluate(arg);
                        didHitReturn = eval.didHitReturn;
                        return value;
                    },
                    [&](const Expression& arg) { return Evaluate(arg); },
                },
                statement.expression);
            if (didHitReturn)
                return value;
        }
        return Value();
    }

    Value Evaluate(const ForExpr&) {
        return Value();
    }

    Value Evaluate(const IfExpr& node) {
        auto ifEval = Evaluator(this, root);
        if (std::get<bool>(ifEval.Evaluate(*node.ifStatement->condition))) {
            auto value = ifEval.Evaluate(*node.ifStatement->block);
            didHitReturn = ifEval.didHitReturn;
            return value;
        }

        for (const auto& elsif : node.elseifStatements) {
            auto elseifEval = Evaluator(this, root);
            if (std::get<bool>(elseifEval.Evaluate(*elsif.condition))) {
                auto value = elseifEval.Evaluate(*elsif.block);
                didHitReturn = elseifEval.didHitReturn;
                return value;
            }
        }

        if (node.elseStatement) {
            auto elseEval = Evaluator(this, root);
            auto value = elseEval.Evaluate(*node.elseStatement->block);
            didHitReturn = elseEval.didHitReturn;
            return value;
        }

        return Value();
    }

    Value Evaluate(const WhileExpr& node) {
        auto whileEval = Evaluator(this, root);
        if (node.isDoWhile) {
            do {
                auto value = whileEval.Evaluate(*node.block);
                if (whileEval.didHitReturn) {
                    didHitReturn = true;
                    return value;
                }
            } while (std::get<bool>(whileEval.Evaluate(*node.condition)));
        } else {
            while (std::get<bool>(whileEval.Evaluate(*node.condition))) {
                auto value = whileEval.Evaluate(*node.block);
                if (whileEval.didHitReturn) {
                    didHitReturn = true;
                    return value;
                }
            }
        }

        return Value();
    }

    Value Evaluate(const Expression& node) {
        return std::visit(
            Visitor{
                [&](const auto&) -> Value { throw; },
                [&](const UnaryOperation& arg) { return Evaluate(arg); },
                [&](const BinaryOperation& arg) { return Evaluate(arg); },
                [&](const VariableRef& arg) { return GetValue(arg.name); },
                [&](const FunctionCall& arg) { return Evaluate(arg); },
                [&](const VariableAssign& arg) { return Evaluate(arg); },
                [&](bool arg) { return Value(arg); },
                [&](double arg) { return Value(arg); },
                [&](const std::string& arg) { return Value(arg); },
                [&](const VariableDef& arg) { return Evaluate(arg); },
            },
            node.expression
        );
    }

    Value Evaluate(const FunctionCall& node) {
        const FunctionDef& func = root.GetFunction(node.name);
        if (node.arguments.size() != func.arguments->arguments.size()) {
            throw TypeMismatchException(std::to_string(node.arguments.size()) + "arg(s)", std::to_string(func.arguments->arguments.size()) + "arg(s)", node.line, "function call with incorrect arguments");
        }

        Evaluator eval(this, root);
        auto inIt = node.arguments.begin();
        auto argIt = func.arguments->arguments.begin();
        for (; inIt != node.arguments.end(); ++inIt, ++argIt) {
            eval.localValues.emplace(argIt->name, Evaluate(*inIt));
        }

        if (!std::holds_alternative<std::monostate>(func.externalFunction)) {
            return eval.Evaluate(func.externalFunction);
        }

        return eval.Evaluate(*func.block);
    }

    Value Evaluate(const VariableDef& node) {
        localValues.emplace(node.name, node.value ? Evaluate(*node.value) : Value());
        return GetValue(node.name);
    }

    Value Evaluate(const VariableAssign& node) {
        auto& value = GetValue(node.name);
        value = Evaluate(*node.value);
        return value;
    }

    Value Evaluate(const BinaryOperation& node) {
        if (node.operation == "==") {
            return Value(Evaluate(*node.lhs) == Evaluate(*node.rhs));
        } else if (node.operation == "!=") {
            return Value(Evaluate(*node.lhs) != Evaluate(*node.rhs));
        } else if (node.operation == "<") {
            return Value(Evaluate(*node.lhs) < Evaluate(*node.rhs));
        } else if (node.operation == "<=") {
            return Value(Evaluate(*node.lhs) <= Evaluate(*node.rhs));
        } else if (node.operation == ">") {
            return Value(Evaluate(*node.lhs) > Evaluate(*node.rhs));
        } else if (node.operation == ">=") {
            return Value(Evaluate(*node.lhs) >= Evaluate(*node.rhs));
        } else if (node.operation == "+") {
            return Evaluate(*node.lhs) + Evaluate(*node.rhs);
        } else if (node.operation == "-") {
            return Value(Evaluate(*node.lhs) - Evaluate(*node.rhs));
        } else if (node.operation == "*") {
            return Value(Evaluate(*node.lhs) * Evaluate(*node.rhs));
        } else if (node.operation == "/") {
            return Value(Evaluate(*node.lhs) / Evaluate(*node.rhs));
        } else if (node.operation == "&&") {
            return Value(Evaluate(*node.lhs) && Evaluate(*node.rhs));
        } else if (node.operation == "||") {
            return Value(Evaluate(*node.lhs) || Evaluate(*node.rhs));
        }

        throw std::runtime_error("Operation " + node.operation + " not implemented.");
    }

    Value Evaluate(const UnaryOperation& node) {
        if (node.operation == "!") {
            return Value(!Evaluate(*node.value));
        } else if (node.operation == "++") {
            return Value(++Evaluate(*node.value));
        } else if (node.operation == "--") {
            return Value(--Evaluate(*node.value));
        }

        throw std::runtime_error("Operation " + node.operation + " not implemented.");
    }

    Value Evaluate(const ExtFunctionType& func) {
        auto inIt = localValues.begin();
        return std::visit(
            Visitor{
                [&](const auto&) -> Value { throw; },
                [&](fVoidStringPtr arg) {
                    arg(std::get<std::string>(inIt++->second));
                    return Value();
                },
                [&](fStringPtr arg) { return Value(arg()); },
                [&](fDoublePtr arg) { return Value(arg()); },
            },
            func
        );
    }
};

} // namespace Interpreter