#include "Ast.hpp"

#include "Lexer.hpp"
#include "WrapperFunctions.hpp"

namespace Interpreter {

const SymbolTable& Node::SymbolsOfType(const SymbolTable& scope, const VarRef& identifier) const {
    if (std::holds_alternative<std::string>(identifier) && !scope.Contains(std::get<std::string>(identifier))) {
        return scope;
    }
    const Symbol& symbol = scope[identifier];
    if (symbol.isFunction || symbol.type <= 5) {
        return scope;
    }
    return GetGlobal().GetObject(symbol.type).Symbols();
}

Global& Node::GetGlobal() {
    Node* actual = this;
    while (actual->parent) {
        actual = actual->parent;
    }
    return *dynamic_cast<Global*>(actual);
}

const Global& Node::GetGlobal() const {
    const Node* actual = this;
    while (actual->parent) {
        actual = actual->parent;
    }
    return *dynamic_cast<const Global*>(actual);
}

IndexOperation::IndexOperation(const SymbolTable& scope, const VarRef& identifier, Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line), identifier(identifier) {
    lSquareOpen::RequireToken(token);
    shift();

    index = std::make_unique<Expression>(this, token, shift);

    lSquareClose::RequireToken(token);
    shift();

    if (ChainedOperation::MatchToken(token)) {
        chainedOperation = std::make_unique<ChainedOperation>(scope, Symbols()["any"].id, this, token, shift);
    }
}

void IndexOperation::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"Index\": {\n";
    index->Print(os, depth + 1);
    if (chainedOperation) {
        chainedOperation->Print(os, depth + 1);
    }
    os << Indent(depth) << "},\n";
}

VarID IndexOperation::ReturnType(const SymbolTable*) const {
    return Symbols()["any"].id;
}

void IndexOperation::SetType(VarID) { }

DotOperation::DotOperation(const SymbolTable& scope, const VarRef& identifier, Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line), scope(scope), identifier(identifier) {
    lDot::RequireToken(token);
    shift();

    attribute = std::visit(
        Visitor{
            [&](VarID arg) { return arg != Symbols()["any"].id ? VarRef(scope[token.text].id) : VarRef(token.text); },
            [&](const std::string& arg) { return scope.Contains(token.text) ? VarRef(scope[token.text].id) : VarRef(token.text); },
        },
        identifier
    );
    shift();

    if (ChainedOperation::MatchToken(token)) {
        chainedOperation = std::make_unique<ChainedOperation>(SymbolsOfType(scope, attribute), attribute, this, token, shift);
    }
}

void DotOperation::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"Dot\": {\n";
    if (std::holds_alternative<VarID>(identifier) && std::get<VarID>(identifier) == Symbols()["any"].id) {
        os << Indent(depth + 1) << "\"Attribute\": " << std::get<std::string>(attribute) << ",\n";
    } else {
        os << Indent(depth + 1) << "\"Attribute\": " << scope[attribute] << ",\n";
    }
    if (chainedOperation) {
        chainedOperation->Print(os, depth + 1);
    }
    os << Indent(depth) << "},\n";
}

VarID DotOperation::ReturnType(const SymbolTable* scope) const {
    const auto symbol = (scope ? *scope : Symbols())[attribute];
    return chainedOperation ? chainedOperation->ReturnType(symbol.isFunction ? nullptr : &GetGlobal().GetObject(symbol.type).Symbols()) : symbol.type;
}

void DotOperation::SetType(VarID type) {
    if (std::holds_alternative<VarID>(identifier) && std::get<VarID>(identifier) == Symbols()["any"].id) {
        return;
    }
    CheckType(scope[attribute].type, type);
}

Range::Range(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    from = std::make_unique<Expression>(this, token, shift);

    if (!lRangeOperator::MatchToken(token)) {
        return;
    }

    lRangeOperator::RequireToken(token);
    shouldIncludeLast = token.ShouldIncludeLast();
    shift();

    to = std::make_unique<Expression>(this, token, shift);
    
    auto anyType = Symbols()["any"].id;
    auto numberType = Symbols()["number"].id;
    if (from->ReturnType() != anyType && from->ReturnType() != numberType) {
        throw TypeMismatchException(ToString(Symbols()[numberType]), ToString(Symbols()[from->ReturnType()]), line, "range must be a number");
    }
    if (to->ReturnType() != anyType && to->ReturnType() != numberType) {
        throw TypeMismatchException(ToString(Symbols()[numberType]), ToString(Symbols()[to->ReturnType()]), line, "range must be a number");
    }
}

void Range::Print(std::ostream& os, size_t depth) const {
    if (to) {
        os << Indent(depth) << "\"From\": {\n";
        from->Print(os, depth + 1);
        os << Indent(depth ) << "},\n";
        os << Indent(depth) << "\"To\": {\n";
        to->Print(os, depth + 1);
        os << Indent(depth) << "},\n";
        os << Indent(depth) << "\"IncludeLast\": " << (shouldIncludeLast ? "true" : "false") << ",\n";
    } else {
        from->Print(os, depth);
    }
}

VarID Range::ReturnType(const SymbolTable* scope) const {
    return (scope ? *scope : Symbols())["range"].id;
}

UnaryOperation::UnaryOperation(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    lUnaryOperator::RequireToken(token);
    operation = token.text;
    shift();

    value = std::make_unique<Expression>(this, token, shift);

    returnType = token.IsLogicalOperator() ? Symbols()["bool"].id : value->ReturnType();
}

void UnaryOperation::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"UnaryOperation\": {\n";
    os << Indent(depth + 1) << "\"Operator\": \"" << operation << "\",\n";
    os << Indent(depth + 1) << "\"Value\": {\n";
    value->Print(os, depth + 2);
    os << Indent(depth + 1) << "},\n";
    os << Indent(depth) << "},\n";
}

VarID UnaryOperation::ReturnType(const SymbolTable* scope) const {
    return returnType;
}

BinaryOperation::BinaryOperation(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    lBinaryOperator::RequireToken(token);
    auto operatorToken = token;
    operation = token.text;
    shift();

    lParenOpen::RequireToken(token);
    shift();

    lhs = std::make_unique<Expression>(this, token, shift);

    lComma::RequireToken(token);
    shift();

    rhs = std::make_unique<Expression>(this, token, shift);

    lParenClose::RequireToken(token);
    shift();

    returnType = operatorToken.IsLogicalOperator() ? Symbols()["bool"].id : lhs->ReturnType();
}

void BinaryOperation::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"BinaryOperation\": {\n";
    os << Indent(depth + 1) << "\"Operator\": \"" << operation << "\",\n";
    os << Indent(depth + 1) << "\"Lhs\": {\n";
    lhs->Print(os, depth + 2);
    os << Indent(depth + 1) << "},\n";
    os << Indent(depth + 1) << "\"Rhs\": {\n";
    rhs->Print(os, depth + 2);
    os << Indent(depth + 1) << "},\n";
    os << Indent(depth) << "},\n";
}

VarID BinaryOperation::ReturnType(const SymbolTable* scope) const {
    return returnType;
}

VariableAssign::VariableAssign(const VarRef& identifier, Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line), identifier(identifier) {
    lBinaryOperator::RequireToken(token);
    shift();

    value = std::make_unique<Expression>(this, token, shift);

    parent->SetType(value->ReturnType());
}

void VariableAssign::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"Assignment\": {\n";
    value->Print(os, depth + 1);
    os << Indent(depth) << "},\n";
}

VarID VariableAssign::ReturnType(const SymbolTable* scope) const {
    return value->ReturnType(scope);
}

VariableRef::VariableRef(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    lIdentifier::RequireToken(token);
    name = Symbols()[token.text].id;
    shift();

    if (ChainedOperation::MatchToken(token)) {
        chainedOperation = std::make_unique<ChainedOperation>(SymbolsOfType(Symbols(), name), name, this, token, shift);
    }
}

void VariableRef::Print(std::ostream& os, size_t depth) const {
    if (!chainedOperation) {
        os << Indent(depth) << "\"Variable\": \"" << Symbols()[name] << "\",\n";
        return;
    }
    os << Indent(depth) << "\"Variable\": {\n";
    os << Indent(depth + 1) << "\"Name\": \"" << Symbols()[name] << "\",\n";
    chainedOperation->Print(os, depth + 1);
    os << Indent(depth) << "},\n";
}

VarID VariableRef::ReturnType(const SymbolTable* scope) const {
    const auto symbol = (scope ? *scope : Symbols())[name];
    return chainedOperation ? chainedOperation->ReturnType(symbol.isFunction ? nullptr : &GetGlobal().GetObject(symbol.type).Symbols()) : symbol.type;
}

void VariableRef::SetType(VarID type) {
    MatchType(name, type);
}

VariableDef::VariableDef(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    lVar::RequireToken(token);
    shift();

    lIdentifier::RequireToken(token);
    name = Symbols().Add(token.text);
    shift();

    if (lColon::MatchToken(token)) {
        shift();
            
        lIdentifier::RequireToken(token);
        const Symbol& type = Symbols()[token.text];
        Symbols().Set(name, type.id, false, type.isArray);
        shift();
    } else {
        Symbols().Set(name, Symbols()["any"].id, false, false);
    }

    if (lSquareOpen::MatchToken(token)) {
        shift();

        isArray = true;

        lSquareClose::RequireToken(token);
        shift();
    }

    if (lBinaryOperator::MatchToken(token) && token.text == "=") {
        shift();

        value = std::make_unique<Expression>(this, token, shift);

        MatchType(name, value->ReturnType());
    }
}

void VariableDef::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"Definition\": {\n";
    os << Indent(depth + 1) << "\"Variable\": \"" << Symbols()[name] << "\",\n";
    os << Indent(depth + 1) << "\"Type\": \"" << Symbols()[Symbols()[name].type] << "\",\n";
    if (value) {
        os << Indent(depth + 1) << "\"Value\": {\n";
        value->Print(os, depth + 2);
        os << Indent(depth + 1) << "},\n";
    }
    os << Indent(depth) << "},\n";
}

VarID VariableDef::ReturnType(const SymbolTable* scope) const {
    return (scope ? *scope : Symbols())[name].type;
}

ObjectInitializer::ObjectInitializer(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    lNew::RequireToken(token);
    shift();
    
    lIdentifier::RequireToken(token);
    type = Symbols()[token.text].id;
    shift();

    lCurlyOpen::RequireToken(token);
    shift();

    const ObjectDef& objectDef = GetGlobal().GetObject(type);
    while (!lCurlyClose::MatchToken(token)) {
        lIdentifier::RequireToken(token);
        VarID variable = objectDef.Symbols()[token.text].id;
        if (values.find(variable) != values.end()) {
            throw IdentifierRedefinitionException(token.text);
        }
        shift();

        lBinaryOperator::RequireToken(token);
        if (token.text != "=") {
            throw ParseException(token, { Token::Type::BinaryOperator });
        }
        shift();

        values.try_emplace(variable, this, token, shift);

        lComma::RequireToken(token);
        shift();
    }

    lCurlyClose::RequireToken(token);
    shift();

    for (const auto& variable : objectDef.attributes) {
        auto expressionIt = values.find(variable.name);
        const Symbol& currentSymbol = objectDef.Symbols()[variable.name];
        if (expressionIt == values.end()) {
            if (!variable.value) {
                throw TypeMismatchException(
                    ToString(Symbols()[type]),
                    "Unknown object",
                    line,
                    "missing " + ToString(currentSymbol) + " of type " + ToString(Symbols()[currentSymbol.type])
                );  
            }
        } else {
            objectDef.CheckType(currentSymbol.type, expressionIt->second.ReturnType());
        }
    }
}

void ObjectInitializer::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"ObjectInit\": {\n";
    os << Indent(depth + 1) << "\"Type\": \"" << Symbols()[type] << "\",\n"; 
    const ObjectDef& objectDef = GetGlobal().GetObject(type);
    for (const auto& pair : values) {
        os << Indent(depth + 1) << "\"" << objectDef.Symbols()[pair.first].name << "\": {\n";
        pair.second.Print(os, depth + 2);
        os << Indent(depth + 1) << "},\n";
    }
    os << Indent(depth) << "},\n";
}

ArrayInitializer::ArrayInitializer(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    lSquareOpen::RequireToken(token);
    shift();
    
    while (!lSquareClose::MatchToken(token)) {
        values.emplace_back(this, token, shift);

        lComma::RequireToken(token);
        shift();
    }

    lSquareClose::RequireToken(token);
    shift();
    
    auto anyType = Symbols()["any"].id;
    type = anyType;
    for (const auto& value : values) {
        auto valueReturnType = value.ReturnType();
        if (valueReturnType != anyType) {
            type = value.ReturnType();
            continue;
        }
        if (type != anyType && type != valueReturnType) {
            throw TypeMismatchException(ToString(Symbols()[type]), ToString(Symbols()[valueReturnType]), value.line);
        }
    }
}

void ArrayInitializer::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"ArrayInit\": {\n";
    os << Indent(depth + 1) << "\"Type\": \"" << Symbols()[type] << "\",\n";
    unsigned i = 0;
    for (const auto& value : values) {
        os << Indent(depth + 1) << "\"" << i++ << "\": {\n";
        value.Print(os, depth + 2);
        os << Indent(depth + 1) << "},\n";
    }
    os << Indent(depth) << "},\n";
}

Arguments::Arguments(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    lParenOpen::RequireToken(token);
    shift();

    if (!lParenClose::MatchToken(token))
        while (true) {
            arguments.emplace_back(this, token, shift);

            if (!lParenClose::MatchToken(token)) {
                lComma::RequireToken(token);
                shift();
            } else {
                break;
            }
        }

    lParenClose::RequireToken(token);
    shift();
}

void Arguments::Print(std::ostream& os, size_t depth) const {
    if (arguments.empty()) {
        os << Indent(depth) << "\"Arguments\": [ ],\n";
        return;
    }
    os << Indent(depth) << "\"Arguments\": [\n";
    for (const auto& arg : arguments) {
        os << Indent(depth + 1) << "{\n";
        arg.Print(os, depth + 2);
        os << Indent(depth + 1) << "},\n";
    }
    os << Indent(depth) << "],\n";
}

FunctionCall::FunctionCall(const SymbolTable& scope, const VarRef& identifier, Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line), identifier(identifier) {
    lParenOpen::RequireToken(token);
    shift();

    if (!lParenClose::MatchToken(token))
        while (true) {
            arguments.emplace_back(this, token, shift);

            if (!lParenClose::MatchToken(token)) {
                lComma::RequireToken(token);
                shift();
            } else {
                break;
            }
        }

    lParenClose::RequireToken(token);
    shift();

    if (ChainedOperation::MatchToken(token)) {
        chainedOperation = std::make_unique<ChainedOperation>(scope, scope[identifier].type, this, token, shift);
    }

    if (std::holds_alternative<VarID>(identifier)) {
        VarID id = std::get<VarID>(identifier);
        // Signature check
        auto node = parent;
        FunctionDef* func = nullptr;
        while (node->parent != nullptr) {
            func = dynamic_cast<FunctionDef*>(node);
            if (func != nullptr && func->name == id) {
                break;
            }
            func = nullptr;
            node = node->parent;
        }
        const FunctionDef& functionDef = func == nullptr ? dynamic_cast<Global*>(node)->GetFunction(id) : *func;
        if (arguments.size() != functionDef.arguments->arguments.size()) {
                throw TypeMismatchException(std::to_string(arguments.size()) + " arg(s)", std::to_string(functionDef.arguments->arguments.size()) + " arg(s)", line);
        }

        auto anyType = Symbols()["any"].id;
        auto inIt = arguments.begin();
        auto argIt = functionDef.arguments->arguments.begin();
        for (; inIt != arguments.end(); ++inIt, ++argIt) {
            auto inType = inIt->ReturnType();
            auto argType = argIt->ReturnType();
            if (inType != anyType && argType != anyType && inType != argType) {
                throw TypeMismatchException(ToString(functionDef.Symbols()[argType]), ToString(Symbols()[inType]), line);
            }
        }
    }
}

void FunctionCall::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"Call\": {\n";
    os << Indent(depth + 1) << "\"Arguments\": {\n";
    for (const auto& arg : arguments) {
        arg.Print(os, depth + 2);
    }
    os << Indent(depth + 1) << "},\n";
    if (chainedOperation) {
        chainedOperation->Print(os, depth + 1);
    }
    os << Indent(depth) << "},\n";
}

VarID FunctionCall::ReturnType(const SymbolTable* scope) const {
    return chainedOperation ? chainedOperation->ReturnType(scope) : (scope ? *scope : Symbols())[identifier].type;
}

ChainedOperation::ChainedOperation(const SymbolTable& scope, const VarRef& identifier, Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    if (DotOperation::MatchToken(token)) {
        operation.emplace<DotOperation>(scope, identifier, this, token, shift);
    } else if (IndexOperation::MatchToken(token)) {
        operation.emplace<IndexOperation>(scope, identifier, this, token, shift);
    } else if (VariableAssign::MatchToken(token)) {
        operation.emplace<VariableAssign>(identifier, this, token, shift);
    } else if (FunctionCall::MatchToken(token)) {
        operation.emplace<FunctionCall>(scope, identifier, this, token, shift);
    } else {
        throw ParseException(token, ExpectedToken());
    }
}

void ChainedOperation::Print(std::ostream& os, size_t depth) const {
    std::visit(
        Visitor{
            [&, depth](const auto&) { os << Indent(depth) << "\"Unknown expression\": null,\n"; },
            [&, depth](const DotOperation& arg) { arg.Print(os, depth); },
            [&, depth](const IndexOperation& arg) { arg.Print(os, depth); },
            [&, depth](const VariableAssign& arg) { arg.Print(os, depth); },
            [&, depth](const FunctionCall& arg) { arg.Print(os, depth); },
        },
        operation
    );
}

VarID ChainedOperation::ReturnType(const SymbolTable* scope) const {
    return std::visit(
        Visitor{
            [&](const auto&) -> VarID { throw InterpreterException("Unknown operation.", line); },
            [&](const DotOperation& arg) { return arg.ReturnType(scope); },
            [&](const IndexOperation& arg) { return arg.ReturnType(scope); },
            [&](const VariableAssign& arg) { return arg.ReturnType(scope); },
            [&](const FunctionCall& arg) { return arg.ReturnType(scope); },
        },
        operation
    );
}

Expression::Expression(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    if (UnaryOperation::MatchToken(token)) {
        expression.emplace<UnaryOperation>(this, token, shift);
    } else if (BinaryOperation::MatchToken(token)) {
        expression.emplace<BinaryOperation>(this, token, shift);
    } else if (VariableRef::MatchToken(token)) {
        expression.emplace<VariableRef>(this, token, shift);
    } else if (VariableDef::MatchToken(token)) {
        expression.emplace<VariableDef>(this, token, shift);
    } else if (ObjectInitializer::MatchToken(token)) {
        expression.emplace<ObjectInitializer>(this, token, shift);
    } else if (ArrayInitializer::MatchToken(token)) {
        expression.emplace<ArrayInitializer>(this, token, shift);
    } else if (lString::MatchToken(token)) {
        lString::RequireToken(token);
        expression = token.text.substr(1, token.text.size() - 2);
        shift();
    } else if (lTrue::MatchToken(token)) {
        lTrue::RequireToken(token);
        expression = true;
        shift();
    } else if (lFalse::MatchToken(token)) {
        lFalse::RequireToken(token);
        expression = false;
        shift();
    } else if (lNumber::MatchToken(token)) {
        lNumber::RequireToken(token);
        expression = std::stod(token.text);
        shift();
    } else {
        throw ParseException(token, ExpectedToken());
    }
}

void Expression::Print(std::ostream& os, size_t depth) const {
    std::visit(
        Visitor{
            [&, depth](const auto&) { os << Indent(depth) << "\"Unknown expression\": null,\n"; },
            [&, depth](const UnaryOperation& arg) { arg.Print(os, depth); },
            [&, depth](const BinaryOperation& arg) { arg.Print(os, depth); },
            [&, depth](const VariableRef& arg) { arg.Print(os, depth); },
            [&, depth](const FunctionCall& arg) { arg.Print(os, depth); },
            [&, depth](const VariableAssign& arg) { arg.Print(os, depth); },
            [&, depth](bool arg) { os << Indent(depth) << "\"Bool\": " << (arg ? "true" : "false") << ",\n"; },
            [&, depth](double arg) { os << Indent(depth) << "\"Number\": " << arg << ",\n"; },
            [&, depth](const std::string& arg) { os << Indent(depth) << "\"String\": \"" << arg << "\",\n"; },
            [&, depth](const VariableDef& arg) { arg.Print(os, depth); },
            [&, depth](const ObjectInitializer& arg) { arg.Print(os, depth); },
        },
        expression
    );
}

VarID Expression::ReturnType(const SymbolTable* scope) const {
    return std::visit(
        Visitor{
            [&](const auto&) -> VarID { throw InterpreterException("Unknown operation.", line); },
            [&](const UnaryOperation& arg) { return arg.ReturnType(scope); },
            [&](const BinaryOperation& arg) { return arg.ReturnType(scope); },
            [&](const VariableRef& arg) { return arg.ReturnType(scope); },
            [&](const FunctionCall& arg) { return arg.ReturnType(scope); },
            [&](const VariableAssign& arg) { return arg.ReturnType(scope); },
            [&](bool) { return Symbols()["bool"].id; },
            [&](double) { return Symbols()["number"].id; },
            [&](const std::string&) { return Symbols()["string"].id; },
            [&](const VariableDef& arg) { return arg.ReturnType(scope); },
            [&](const ObjectInitializer& arg) { return arg.ReturnType(scope); },
            [&](const ArrayInitializer& arg) { return arg.ReturnType(scope); },
        },
        expression
    );
}

WhileExpr::WhileExpr(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line), symbols(&parent->Symbols()) {
    isDoWhile = lDo::MatchToken(token);
    if (isDoWhile) {
        lDo::RequireToken(token);
        shift();

        block = std::make_unique<Block>(this, token, shift);

        lWhile::RequireToken(token);
        shift();

        condition = std::make_unique<Expression>(this, token, shift);

        lSemicolon::RequireToken(token);
        shift();
        return;
    }

    lWhile::RequireToken(token);
    shift();

    condition = std::make_unique<Expression>(this, token, shift);
    block = std::make_unique<Block>(this, token, shift);
}

void WhileExpr::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << (isDoWhile ? "\"DoWhile\": {\n" : "\"While\": {\n");
    os << Indent(depth + 1) << "\"Symbols\": " << Symbols() << ",\n";
    os << Indent(depth + 1) << "\"Condition\": {\n";
    condition ? condition->Print(os, depth + 2) : void();
    os << Indent(depth + 1) << "},\n";
    block ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "},\n";
}

VarID WhileExpr::ReturnType(const SymbolTable* scope) const {
    return block->ReturnType(scope);
}

Else::Else(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line), symbols(&parent->Symbols()) {
    lElse::RequireToken(token);
    shift();

    block = std::make_unique<Block>(this, token, shift);
}

void Else::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"Else\": {\n";
    os << Indent(depth + 1) << "\"Symbols\": " << Symbols() << ",\n";
    block ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "},\n";
}

VarID Else::ReturnType(const SymbolTable* scope) const {
    return block->ReturnType(scope);
}

Elseif::Elseif(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line), symbols(&parent->Symbols()) {
    lElseif::RequireToken(token);
    shift();

    condition = std::make_unique<Expression>(this, token, shift);
    block = std::make_unique<Block>(this, token, shift);
}

void Elseif::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "{\n";
    os << Indent(depth + 1) << "\"Symbols\": " << Symbols() << ",\n";
    os << Indent(depth + 1) << "\"Condition\": {\n";
    condition ? condition->Print(os, depth + 2) : void();
    os << Indent(depth + 1) << "},\n";
    block ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "},\n";
}

VarID Elseif::ReturnType(const SymbolTable* scope) const {
    return block->ReturnType(scope);
}

If::If(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line), symbols(&parent->Symbols()) {
    lIf::RequireToken(token);
    shift();

    condition = std::make_unique<Expression>(this, token, shift);
    block = std::make_unique<Block>(this, token, shift);
}

void If::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"If\": {\n";
    os << Indent(depth + 1) << "\"Symbols\": " << Symbols() << ",\n";
    os << Indent(depth + 1) << "\"Condition\": {\n";
    condition ? condition->Print(os, depth + 2) : void();
    os << Indent(depth + 1) << "},\n";
    block ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "},\n";
}

VarID If::ReturnType(const SymbolTable* scope) const {
    return block->ReturnType(scope);
}

IfExpr::IfExpr(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    ifStatement = std::make_unique<If>(this, token, shift);

    while (Elseif::MatchToken(token)) {
        elseifStatements.emplace_back(this, token, shift);
    }

    if (Else::MatchToken(token)) {
        elseStatement = std::make_unique<Else>(this, token, shift);
    }

    returnType = ifStatement->block->HasReturn() ? ifStatement->block->ReturnType() : 0u;
    VarID anyType = Symbols()["any"].id;
    for (const auto& elsif : elseifStatements) {
        if (!elsif.block->HasReturn()) {
            continue;
        }
        VarID blockReturn = elsif.block->ReturnType();
        if (returnType == anyType || returnType == 0) {
            returnType = blockReturn;
        }
        if (returnType != blockReturn && blockReturn != anyType) {
            throw TypeMismatchException(ToString(Symbols()[returnType]), ToString(Symbols()[blockReturn]), elsif.line, "all blocks of elseif statement must have same return type");
        }
    }

    if (elseStatement && elseStatement->block->HasReturn()) {
        VarID blockReturn = elseStatement->block->ReturnType();
        if (returnType == anyType || returnType == 0) {
            returnType = blockReturn;
        }
        if (returnType != blockReturn && blockReturn != anyType) {
            throw TypeMismatchException(ToString(Symbols()[returnType]), ToString(Symbols()[blockReturn]), elseStatement->line, "all blocks of elseif statement must have same return type");
        }
    }
    if (returnType == 0) {
        returnType = Symbols()["void"].id;
    }
}

void IfExpr::Print(std::ostream& os, size_t depth) const {
    ifStatement->Print(os, depth);
    if (!elseifStatements.empty()) {
        os << Indent(depth) << "\"ElseIf\": [\n";
        for (const auto& statement : elseifStatements) {
            statement.Print(os, depth + 1);
        }
        os << Indent(depth) << "],\n";
    }
    elseStatement ? elseStatement->Print(os, depth) : void();
}

VarID IfExpr::ReturnType(const SymbolTable* scope) const {
    return returnType;
}

ForExpr::ForExpr(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line), symbols(&parent->Symbols()) {
    lFor::RequireToken(token);
    shift();

    controlVariable = std::make_unique<VariableDef>(this, token, shift);
    if (controlVariable->value) {
        throw InterpreterException("For loop control variable can't have an assigned value.", line);
    }

    lIn::RequireToken(token);
    shift();

    range = std::make_unique<Range>(this, token, shift);
    block = std::make_unique<Block>(this, token, shift);
}

void ForExpr::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"For\": {\n";
    os << Indent(depth + 1) << "\"Symbols\": " << Symbols() << ",\n";
    os << Indent(depth + 1) << "\"ControlVariable\": {\n";
    controlVariable ? controlVariable->Print(os, depth + 2) : void();
    os << Indent(depth + 1) << "},\n";
    os << Indent(depth + 1) << "\"In\": {\n";
    range ? range->Print(os, depth + 2) : void();
    os << Indent(depth + 1) << "},\n";
    block ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "},\n";
}

VarID ForExpr::ReturnType(const SymbolTable* scope) const {
    return block->ReturnType(scope);
}

Return::Return(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    lReturn::RequireToken(token);
    shift();

    if (!lSemicolon::MatchToken(token)) {
        value = std::make_unique<Expression>(this, token, shift);
    }

    lSemicolon::RequireToken(token);
    shift();
}

void Return::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"Return\": {\n";
    value ? value->Print(os, depth + 1) : void();
    os << Indent(depth) << "},\n";
}

VarID Return::ReturnType(const SymbolTable* scope) const {
    return value ? value->ReturnType(scope) : Symbols()["void"].id;
}

Statement::Statement(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    if (Return::MatchToken(token)) {
        expression.emplace<Return>(this, token, shift);
    } else if (ForExpr::MatchToken(token)) {
        expression.emplace<ForExpr>(this, token, shift);
    } else if (IfExpr::MatchToken(token)) {
        expression.emplace<IfExpr>(this, token, shift);
    } else if (WhileExpr::MatchToken(token)) {
        expression.emplace<WhileExpr>(this, token, shift);
    } else if (Expression::MatchToken(token)) {
        expression.emplace<Expression>(this, token, shift);

        lSemicolon::RequireToken(token);
        shift();
    } else {
        throw ParseException(token, ExpectedToken());
    }
}

void Statement::Print(std::ostream& os, size_t depth) const {
    std::visit(
        Visitor{
            [&, depth](const auto&) { os << Indent(depth) << "\"Unknown statement\": null,\n"; },
            [&, depth](const Return& arg) { arg.Print(os, depth); },
            [&, depth](const ForExpr& arg) { arg.Print(os, depth); },
            [&, depth](const IfExpr& arg) { arg.Print(os, depth); },
            [&, depth](const WhileExpr& arg) { arg.Print(os, depth); },
            [&, depth](const Expression& arg) { arg.Print(os, depth); },
        },
        expression
    );
}

VarID Statement::ReturnType(const SymbolTable* scope) const {
    return std::visit(
        Visitor{
            [&](const auto&) -> VarID { throw InterpreterException("Unknown operation.", line); },
            [&](const Return& arg) { return arg.ReturnType(scope); },
            [&](const ForExpr& arg) { return arg.ReturnType(scope); },
            [&](const IfExpr& arg) { return arg.ReturnType(scope); },
            [&](const WhileExpr& arg) { return arg.ReturnType(scope); },
            [&](const Expression&) { return 0u; },
        },
        expression
    );
}

bool Statement::HasReturn() const {
    return std::visit(
        Visitor{
            [&](const auto&) -> bool { throw InterpreterException("Unknown operation.", line); },
            [&](const Return&) { return true; },
            [&](const ForExpr& arg) { return arg.block->HasReturn(); },
            [&](const IfExpr& arg) {
                if (arg.ifStatement->block->HasReturn()) {
                    return true;
                }

                for (const auto& elseif : arg.elseifStatements) {
                    if (elseif.block->HasReturn()) {
                        return true;
                    }
                }

                return arg.elseStatement && arg.elseStatement->block->HasReturn();
            },
            [&](const WhileExpr& arg) { return arg.block->HasReturn(); },
            [&](const Expression&) { return false; },
        },
        expression
    );
}

Block::Block(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line) {
    lCurlyOpen::RequireToken(token);
    shift();

    while (!lCurlyClose::MatchToken(token)) {
        statements.emplace_back(this, token, shift);
    }

    lCurlyClose::RequireToken(token);
    shift();

    // Check return types
    auto anyType = Symbols()["any"].id;
    auto voidType = Symbols()["void"].id;
    returnType = 0u;
    for (const auto& statement : statements) {
        auto current = std::visit(
            Visitor{
                [&](const auto&) -> VarID { throw InterpreterException("Unknown statement.", statement.line); },
                [&](const Return& arg) { return arg.ReturnType(); },
                [&](const ForExpr& arg) { return arg.ReturnType(); },
                [&](const IfExpr& arg) { return arg.ReturnType(); },
                [&](const WhileExpr& arg) { return arg.ReturnType(); },
                [&](const Expression& arg) { return 0u; },
            },
            statement.expression
        );
        if (!statement.HasReturn()) {
            continue;
        }
        if (returnType == 0 || returnType == anyType) {
            returnType = current;
        }
        if (returnType != current && current != anyType) {
            throw TypeMismatchException(ToString(Symbols()[returnType]), ToString(Symbols()[current]), statement.line, "wrong return type");
        }
    }
    if (returnType == 0) {
        returnType = voidType;
    }
}

void Block::Print(std::ostream& os, size_t depth) const {
    if (statements.empty()) {
        os << Indent(depth) << "\"Block\": [ ],\n";
        return;
    }
    os << Indent(depth) << "\"Block\": [\n";
    for (const auto& statement : statements) {
        os << Indent(depth + 1) << "{\n";
        statement.Print(os, depth + 2);
        os << Indent(depth + 1) << "},\n";
    }
    os << Indent(depth) << "],\n";
}

VarID Block::ReturnType(const SymbolTable* scope) const {
    return returnType;
}

bool Block::HasReturn() const {
    for (const auto& statement : statements) {
        if (statement.HasReturn()) {
            return true;
        }
    }
    return false;
}

FunctionDef::FunctionDef(Node* parent, const std::string& signature, ExtFunctionType&& function)
    : Node(parent, 0), symbols(&parent->Symbols()), externalFunction(std::move(function)) {
    
    std::istringstream source(signature.c_str());
    Lexer lexer(source);
    auto token = lexer.Next();
    auto shift = [&]() { token = lexer.Next(); };

    lFunc::RequireToken(token);
    shift();

    lIdentifier::RequireToken(token);
    name = parent->Symbols().Add(token.text);
    shift();

    arguments = std::make_unique<Arguments>(this, token, shift);
    
    if (lColon::MatchToken(token)) {
        shift();
            
        lIdentifier::RequireToken(token);
        const Symbol& returnType = Symbols()[token.text];
        Symbols().Set(name, returnType.id, true, returnType.isArray);
        shift();
    } else {
        Symbols().Set(name, Symbols()["any"].id, true, false);
    }
}

FunctionDef::FunctionDef(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line), symbols(&parent->Symbols()) {
    lFunc::RequireToken(token);
    shift();

    lIdentifier::RequireToken(token);
    name = parent->Symbols().Add(token.text);
    shift();

    arguments = std::make_unique<Arguments>(this, token, shift);
    
    if (lColon::MatchToken(token)) {
        shift();
            
        lIdentifier::RequireToken(token);
        const Symbol& returnType = Symbols()[token.text];
        Symbols().Set(name, returnType.id, true, returnType.isArray);
        shift();
    } else {
        Symbols().Set(name, Symbols()["any"].id, true, false);
    }

    block = std::make_unique<Block>(this, token, shift);
    if (block->ReturnType() == 0) {
        block->returnType = Symbols()["void"].id;
    }

    MatchType(name, block->ReturnType(), "wrong return type");
}

void FunctionDef::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"FunctionDef\": {\n";
    os << Indent(depth + 1) << "\"Name\": \"" << Symbols()[name] << "\",\n";
    os << Indent(depth + 1) << "\"Symbols\": " << Symbols() << ",\n";
    arguments ? arguments->Print(os, depth + 1) : void();
    os << Indent(depth + 1) << "\"Returns\": \"" << Symbols()[Symbols()[name].type] << "\",\n";
    block ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "},\n";
}

VarID FunctionDef::ReturnType(const SymbolTable* scope) const {
    return (scope ? *scope : Symbols())[name].type;
}

ObjectDef::ObjectDef(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent, token.line), symbols(&parent->Symbols()) {
    lObject::RequireToken(token);
    shift();

    lIdentifier::RequireToken(token);
    name = parent->Symbols().Add(token.text);
    shift();

    lCurlyOpen::RequireToken(token);
    shift();

    while (VariableDef::MatchToken(token)) {
        attributes.emplace_back(this, token, shift);

        lSemicolon::RequireToken(token);
        shift();
    }

    lCurlyClose::RequireToken(token);
    shift();
}

void ObjectDef::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "\"Object\": {\n";
    os << Indent(depth + 1) << "\"Name\": \"" << Symbols()[name] << "\",\n";
    os << Indent(depth + 1) << "\"Symbols\": " << Symbols() << ",\n";
    if (attributes.empty()) {
        os << Indent(depth + 1) << "\"Attributes\": [ ],\n";
    } else {
        os << Indent(depth + 1) << "\"Attributes\": [\n";
        for (const auto& attribute : attributes) {
            os << Indent(depth + 2) << "{\n";
            attribute.Print(os, depth + 3);
            os << Indent(depth + 2) << "},\n";
        }
        os << Indent(depth + 1) << "],\n";
    }
    os << Indent(depth) << "},\n";
}

Global::Global(const Token& token, const std::function<void()>& shift)
    : Node(nullptr, 0), symbols(nullptr) {
    // Add predefined symbols and functions
    symbols.Add("any");
    symbols.Add("void");
    symbols.Add("bool");
    symbols.Add("string");
    symbols.Add("number");
    
    definitions.emplace_back(std::in_place_type<FunctionDef>, this, "func Write(var message): void", &Write);
    definitions.emplace_back(std::in_place_type<FunctionDef>, this, "func WriteLine(var message): void", &WriteLine);
    definitions.emplace_back(std::in_place_type<FunctionDef>, this, "func ReadNumber(): number", &ReadNumber);
    definitions.emplace_back(std::in_place_type<FunctionDef>, this, "func ReadText(): string", &ReadText);

    try {
        while (!lEoF::MatchToken(token)) {
            if (FunctionDef::MatchToken(token)) {
                definitions.emplace_back(std::in_place_type<FunctionDef>, this, token, shift);
            } else if (ObjectDef::MatchToken(token)) {
                definitions.emplace_back(std::in_place_type<ObjectDef>, this, token, shift);
            } else {
                throw ParseException(token, RuleGroup<FunctionDef, ObjectDef>::ExpectedToken());
            }
        }
    } catch (const UndefinedIdentifierException& err) {
        throw InterpreterException(err.what(), token.line);
    } catch (const IdentifierRedefinitionException& err) {
        throw InterpreterException(err.what(), token.line);
    }
}

void Global::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "{\n";
    os << Indent(depth + 1) << "\"Symbols\": " << Symbols() << ",\n";
    os << Indent(depth + 1) << "\"Definitions\": [\n";
    for (const auto& definition : definitions) {
        os << Indent(depth + 2) << "{\n";
        std::visit(
            Visitor {
                [&, depth](const auto&) { os << Indent(depth + 3) << "\"Unknown definition\": null,\n"; },
                [&, depth](const FunctionDef& arg) { arg.Print(os, depth + 3); },
                [&, depth](const ObjectDef& arg) { arg.Print(os, depth + 3); },
            },
            definition
        );
        os << Indent(depth + 2) << "},\n";
    }
    os << Indent(depth + 1) << "],\n";
    os << Indent(depth) << "}\n";
}

} // namespace Interpreter
