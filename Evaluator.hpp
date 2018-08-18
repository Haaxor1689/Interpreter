#pragma once

#include <map>
#include <variant>

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
            throw InterpreterException("Wrong number of arguments.", func.line);
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
        throw InternalException("Variable with id " + std::to_string(variable) + " not found.");
    }

    Value Evaluate(const Block& node) {
        for (const Statement& statement : node.statements) {
            Value value = std::visit(
                Visitor{
                    [&](const auto&) -> Value { throw InterpreterException("Unknown operation.", node.line); },
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

    Value Evaluate(const ForExpr& node) {
        auto forEval = Evaluator(this, root);

        auto from = std::get<double>(forEval.Evaluate(*node.range->from));
        auto to = std::get<double>(forEval.Evaluate(*node.range->to));

        localValues.emplace(node.controlVariable->name, from);
        auto& controlVariable = GetValue(node.controlVariable->name);
        auto end = [&](double i) {
            if (from < to) {
                return i < (node.range->shouldIncludeLast ? to + 1 : to);
            }
            return i > (node.range->shouldIncludeLast ? to - 1 : to);
        };

        for (double i = from; end(i); from < to ? ++i : --i) {
            controlVariable = Value(i);
            auto value = forEval.Evaluate(*node.block);
            if (forEval.didHitReturn) {
                didHitReturn = true;
                return value;
            }
        }

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
                [&](const auto&) -> Value { throw InterpreterException("Unknown operation.", node.line); },
                [&](const UnaryOperation& arg) { return Evaluate(arg); },
                [&](const BinaryOperation& arg) { return Evaluate(arg); },
                [&](const VariableRef& arg) { return Evaluate(arg); },
                [&](const VariableDef& arg) { return Evaluate(arg); },
                [&](const ObjectInitializer& arg) { return Evaluate(arg); },
                [&](const ArrayInitializer& arg) { return Evaluate(arg); },
                [&](bool arg) { return Value(arg); },
                [&](double arg) { return Value(arg); },
                [&](const std::string& arg) { return Value(arg); },
            },
            node.expression
        );
    }

    Value Evaluate(const ChainedOperation& node, Value* value) {
        return std::visit(
            Visitor{
                [&](const auto&) -> Value { throw InterpreterException("Unknown operation.", node.line); },
                [&](const DotOperation& arg) { return Evaluate(arg, value); },
                [&](const IndexOperation& arg) { return Evaluate(arg, value); },
                [&](const VariableAssign& arg) { return Evaluate(arg, value); },
                [&](const FunctionCall& arg) { return Evaluate(arg); },
            },
            node.operation
        );
    }

    Value Evaluate(const FunctionCall& node) {
        const FunctionDef& func = root.GetFunction(node.identifier);
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

        auto value = eval.Evaluate(*func.block);
        if (node.chainedOperation) {
            return Evaluate(*node.chainedOperation, &value);
        }
        return value;
    }

    Value Evaluate(const VariableDef& node) {
        localValues.emplace(node.name, node.value ? Evaluate(*node.value) : Value());
        try {
            return GetValue(node.name);
        } catch (const InternalException& err) {
            throw InterpreterException(err.what(), node.line);
        }
    }

    Value Evaluate(const VariableRef& node) {
        if (node.Symbols()[node.name].isFunction) {
            return Evaluate(*node.chainedOperation, nullptr);
        }
        try {
            auto& value = GetValue(node.name);
            if (node.chainedOperation) {
                return Evaluate(*node.chainedOperation, &value);
            }
            return value;
        } catch (const InternalException& err) {
            throw InterpreterException(err.what(), node.line);
        }
    }

    Value Evaluate(const DotOperation& node, Value* value) {
        if (node.Symbols()[node.identifier].isFunction) {
            return Evaluate(*node.chainedOperation, nullptr);
        }
        try {
            auto& localValue = std::get<Object>(*value).values[node.scope[node.attribute].name];
            if (node.chainedOperation) {
                return Evaluate(*node.chainedOperation, &localValue);
            }
            return localValue;
        } catch (const InternalException& err) {
            throw InterpreterException(err.what(), node.line);
        }
    }

    Value Evaluate(const IndexOperation& node, Value* value) {
        throw InterpreterException("Not implemented.", node.line);
    }

    Value Evaluate(const VariableAssign& node, Value* value) {
        try {
            *value = Evaluate(*node.value);
            return value;
        } catch (const InternalException& err) {
            throw InterpreterException(err.what(), node.line);
        }
    }

    Value Evaluate(const BinaryOperation& node) {
        try {
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
        } catch (const InternalException& err) {
            throw InterpreterException(err.what(), node.line);
        }
        throw InterpreterException("Operator " + node.operation + " not implemented.", node.line);
    }

    Value Evaluate(const UnaryOperation& node) {
        try {
            if (node.operation == "!") {
                return Value(!Evaluate(*node.value));
            } else if (node.operation == "++") {
                return Value(++Evaluate(*node.value));
            } else if (node.operation == "--") {
                return Value(--Evaluate(*node.value));
            }
        } catch (const InternalException& err) {
            throw InterpreterException(err.what(), node.line);
        }
        throw InterpreterException("Operator " + node.operation + " not implemented.", node.line);
    }

    Value Evaluate(const ExtFunctionType& func) {
        auto inIt = localValues.begin();
        return std::visit(
            Visitor{
                [&](const auto&) -> Value { throw InterpreterException("Unknown operation.", 0); },
                [&](fVoidValuePtr arg) {
                    arg(inIt++->second);
                    return Value();
                },
                [&](fStringPtr arg) { return Value(arg()); },
                [&](fDoublePtr arg) { return Value(arg()); },
            },
            func
        );
    }

    Value Evaluate(const ObjectInitializer& node) {
        const ObjectDef& objectDef = root.GetObject(node.type);
        std::map<std::string, Value> values;

        for (const auto& attribute : objectDef.attributes) {
            auto expressionIt = node.values.find(attribute.name);
            if (expressionIt == node.values.end()) {
                values[objectDef.Symbols()[attribute.name].name] = Evaluate(*attribute.value);
            } else {
                values[objectDef.Symbols()[attribute.name].name] = Evaluate(node.values.find(attribute.name)->second);
            }
        }

        return Value(std::move(values));
    }

    Value Evaluate(const ArrayInitializer& node) {
        std::vector<Value> values;

        for (const auto& value : node.values) {
            values.push_back(Evaluate(value));
        }

        return Value(std::move(values));
    }
};

} // namespace Interpreter