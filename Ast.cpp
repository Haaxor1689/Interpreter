#include "Ast.hpp"

#include "Lexer.hpp"
#include "WrapperFunctions.hpp"

namespace Interpreter {

BinaryOperation::BinaryOperation(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lBinaryOperator::RequireToken(token);
    operation = token.text;
    shift();

    lhs = std::make_unique<Expression>(this, token, shift);
    rhs = std::make_unique<Expression>(this, token, shift);
}

void BinaryOperation::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "BinaryOperation: {\n";
    os << Indent(depth + 1) << "Operator: " << operation << "\n";
    os << Indent(depth + 1) << "Lhs: {\n";
    lhs->Print(os, depth + 2);
    os << Indent(depth + 1) << "}\n";
    os << Indent(depth + 1) << "Rhs: {\n";
    rhs->Print(os, depth + 2);
    os << Indent(depth + 1) << "}\n";
    os << Indent(depth) << "}\n";
}

VarID BinaryOperation::ReturnType() const {
    return lhs->ReturnType();
}

VariableAssign::VariableAssign(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lBinaryOperator::RequireToken(token);
    shift();

    value = std::make_unique<Expression>(this, token, shift);

    auto anyType = Symbols().GetSymbol("any").id;
    auto expressionType = value->ReturnType();
    if (Symbols().GetType(name) == anyType) {
        Symbols().SetType(name, expressionType);
    }
    if (expressionType != anyType && Symbols().GetType(name) != expressionType) {
        throw TypeMismatchException(Symbols().GetName(Symbols().GetType(name)), Symbols().GetName(expressionType));
    }
}

void VariableAssign::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Assignment: {\n";
    os << Indent(depth + 1) << "Variable: " << Symbols().GetName(name) << "\n";
    os << Indent(depth + 1) << "Value: {\n";
    value->Print(os, depth + 2);
    os << Indent(depth + 1) << "}\n";
    os << Indent(depth) << "}\n";
}

VarID VariableAssign::ReturnType() const {
    return Symbols().GetType(name);
}

TypeName::TypeName(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    
    if (lColon::MatchToken(token)) {
        shift();
            
        lIdentifier::RequireToken(token);
        typeName = Symbols().GetSymbol(token.text).id;
        shift();
    } else {
        typeName = Symbols().GetSymbol("any").id;
    }
}

void TypeName::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Type: " << Symbols().GetName(typeName) << "\n";
}

VarID TypeName::ReturnType() const {
    return typeName;
}

VariableRef::VariableRef(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lIdentifier::RequireToken(token);
    name = Symbols().GetSymbol(token.text).id;
    shift();
}

void VariableRef::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Variable: " << Symbols().GetName(name) << "\n";
}

VarID VariableRef::ReturnType() const {
    return Symbols().GetType(name);
}

VariableDef::VariableDef(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lVar::RequireToken(token);
    shift();

    lIdentifier::RequireToken(token);
    name = Symbols().AddSymbol(token.text);
    shift();

    type = std::make_unique<TypeName>(this, token, shift);

    if (lBinaryOperator::MatchToken(token) && token.text == "=") {
        shift();

        value = std::make_unique<Expression>(this, token, shift);

        auto anyType = Symbols().GetSymbol("any").id;
        if (ReturnType() == anyType) {
            type->typeName = value->ReturnType();
        } else if (ReturnType() != value->ReturnType() && value->ReturnType() != anyType) {
            throw TypeMismatchException(Symbols().GetName(ReturnType()), Symbols().GetName(value->ReturnType()));
        }
    }
    
    Symbols().SetType(name, ReturnType());
}

void VariableDef::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Definition: {\n";
    os << Indent(depth + 1) << "Variable: " << Symbols().GetName(name) << "\n";
    if (type) {
        type->Print(os, depth + 1);
    }
    if (value) {
        os << Indent(depth + 1) << "Value: {\n";
        value->Print(os, depth + 2);
        os << Indent(depth + 1) << "}\n";
    }
    os << Indent(depth) << "}\n";
}

VarID VariableDef::ReturnType() const {
    return type->typeName;
}

Arguments::Arguments(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
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
    
    returnType = std::make_unique<TypeName>(this, token, shift);
}

void Arguments::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Arguments: {\n";
    for (const auto& arg : arguments)
        arg.Print(os, depth + 1);
    os << Indent(depth) << "}\n";
    if (returnType) {
        os << Indent(depth) << "Returns: {\n";
        returnType->Print(os, depth + 1);
        os << Indent(depth) << "}\n";
    }
}

FunctionCall::FunctionCall(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
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

    // Signature check
    auto node = parent;
    FunctionDef* func = nullptr;
    while (node->parent != nullptr) {
        func = dynamic_cast<FunctionDef*>(node);
        if (func != nullptr && func->name == name) {
            break;
        }
        func = nullptr;
        node = node->parent;
    }
    const FunctionDef& functionDef = func == nullptr ? static_cast<Global*>(node)->GetFunction(name) : *func;
    if (arguments.size() != functionDef.arguments->arguments.size()) {
            throw TypeMismatchException(std::to_string(arguments.size()) + " arg(s)", std::to_string(functionDef.arguments->arguments.size()) + " arg(s)");
    }

    auto anyType = Symbols().GetSymbol("any").id;
    auto inIt = arguments.begin();
    auto argIt = functionDef.arguments->arguments.begin();
    for (; inIt != arguments.end(); ++inIt, ++argIt) {
        auto inType = inIt->ReturnType();
        auto argType = argIt->ReturnType();
        if (inType != anyType && argType != anyType && inType != argType) {
            throw TypeMismatchException(functionDef.Symbols().GetName(argType), Symbols().GetName(inType));
        }
    }
}

void FunctionCall::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "FunctionCall: {\n";
    os << Indent(depth + 1) << "Name: " << Symbols().GetName(name) << "\n";
    os << Indent(depth + 1) << "Arguments: {\n";
    for (const auto& arg : arguments)
        arg.Print(os, depth + 2);
    os << Indent(depth + 1) << "}\n";
    os << Indent(depth) << "}\n";
}

VarID FunctionCall::ReturnType() const {
    return Symbols().GetType(name);
}

Expression::Expression(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    if (lBinaryOperator::MatchToken(token)) {
        lBinaryOperator::RequireToken(token);
        expression.emplace<BinaryOperation>(this, token, shift);
    } else if (lIdentifier::MatchToken(token)) {
        lIdentifier::RequireToken(token);
        expression.emplace<VariableRef>(this, token, shift);

        if (FunctionCall::MatchToken(token)) {
            VarID name = std::get<VariableRef>(expression).name;
            expression.emplace<FunctionCall>(this, token, shift);
            std::get<FunctionCall>(expression).name = name;
        } else if (VariableAssign::MatchToken(token)) {
            VarID name = std::get<VariableRef>(expression).name;
            expression.emplace<VariableAssign>(this, token, shift);
            std::get<VariableAssign>(expression).name = name;
        }
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
    } else if (VariableDef::MatchToken(token)) {
        expression.emplace<VariableDef>(this, token, shift);
    } else {
        throw ParseException(token, ExpectedToken());
    }
}

void Expression::Print(std::ostream& os, size_t depth) const {
    std::visit(
        Visitor{
            [&, depth](const auto&) { os << Indent(depth) << "Unknown expression\n"; },
            [&, depth](const BinaryOperation& arg) { arg.Print(os, depth); },
            [&, depth](const VariableRef& arg) { arg.Print(os, depth); },
            [&, depth](const FunctionCall& arg) { arg.Print(os, depth); },
            [&, depth](const VariableAssign& arg) { arg.Print(os, depth); },
            [&, depth](bool arg) { os << Indent(depth) << "Bool: " << (arg ? "true" : "false") << "\n"; },
            [&, depth](double arg) { os << Indent(depth) << "Double: " << arg << "\n"; },
            [&, depth](const std::string& arg) { os << Indent(depth) << "String: \"" << arg << "\"\n"; },
            [&, depth](const VariableDef& arg) { arg.Print(os, depth); },
        },
        expression
    );
}

VarID Expression::ReturnType() const {
    return std::visit(
        Visitor{
            [&](const auto&) -> VarID { throw; },
            [&](const BinaryOperation& arg) { return arg.ReturnType(); },
            [&](const VariableRef& arg) { return arg.ReturnType(); },
            [&](const FunctionCall& arg) { return arg.ReturnType(); },
            [&](const VariableAssign& arg) { return arg.ReturnType(); },
            [&](bool arg) { return Symbols().GetSymbol("bool").id; },
            [&](double arg) { return Symbols().GetSymbol("number").id; },
            [&](const std::string& arg) { return Symbols().GetSymbol("string").id; },
            [&](const VariableDef& arg) { return arg.ReturnType(); },
        },
        expression
    );
}

WhileExpr::WhileExpr(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent), symbols(&parent->Symbols()) {
    if ((isDoWhile = lDo::MatchToken(token))) {
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
    os << Indent(depth) << (isDoWhile ? "DoWhile: {\n" : "While: {\n");
    os << Indent(depth + 1) << "Symbols: " << Symbols() << "\n";
    os << Indent(depth + 1) << "Condition: {\n";
    condition ? condition->Print(os, depth + 2) : void();
    os << Indent(depth + 1) << "}\n";
    block ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "}\n";
}

VarID WhileExpr::ReturnType() const {
    return block->ReturnType();
}

Else::Else(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent), symbols(&parent->Symbols()) {
    lElse::RequireToken(token);
    shift();

    block = std::make_unique<Block>(this, token, shift);
}

void Else::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Else: {\n";
    os << Indent(depth + 1) << "Symbols: " << Symbols() << "\n";
    block ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "}\n";
}

VarID Else::ReturnType() const {
    return block->ReturnType();
}

Elseif::Elseif(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent), symbols(&parent->Symbols()) {
    lElseif::RequireToken(token);
    shift();

    condition = std::make_unique<Expression>(this, token, shift);
    block = std::make_unique<Block>(this, token, shift);
}

void Elseif::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Elseif: {\n";
    os << Indent(depth + 1) << "Symbols: " << Symbols() << "\n";
    os << Indent(depth + 1) << "Condition: {\n";
    condition ? condition->Print(os, depth + 2) : void();
    os << Indent(depth + 1) << "}\n";
    block ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "}\n";
}

VarID Elseif::ReturnType() const {
    return block->ReturnType();
}

If::If(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent), symbols(&parent->Symbols()) {
    lIf::RequireToken(token);
    shift();

    condition = std::make_unique<Expression>(this, token, shift);
    block = std::make_unique<Block>(this, token, shift);
}

void If::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "If: {\n";
    os << Indent(depth + 1) << "Symbols: " << Symbols() << "\n";
    os << Indent(depth + 1) << "Condition: {\n";
    condition ? condition->Print(os, depth + 2) : void();
    os << Indent(depth + 1) << "}\n";
    block ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "}\n";
}

VarID If::ReturnType() const {
    return block->ReturnType();
}

IfExpr::IfExpr(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    ifStatement = std::make_unique<If>(this, token, shift);
    if (!lElse::MatchToken(token) && !lElseif::MatchToken(token))
        return;

    while (!lElse::MatchToken(token)) {
        elseifStatements.emplace_back(this, token, shift);
    }
    elseStatement = std::make_unique<Else>(this, token, shift);
}

void IfExpr::Print(std::ostream& os, size_t depth) const {
    ifStatement->Print(os, depth);
    for (const auto& statement : elseifStatements) {
        statement.Print(os, depth);
    }
    elseStatement ? elseStatement->Print(os, depth) : void();
}

VarID IfExpr::ReturnType() const {
    VarID returnType = 0;

    returnType = ifStatement->block->ReturnType();
    if (returnType != 0) {
        return returnType;
    }

    for (const auto& elsif : elseifStatements) {
        returnType = elsif.block->ReturnType();
        if (returnType != 0) {
            return returnType;
        }
    }

    if (elseStatement) {
        returnType = elseStatement->block->ReturnType();
        if (returnType != 0) {
            return returnType;
        }
    }

    return 0;
}

ForExpr::ForExpr(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent), symbols(&parent->Symbols()) {
    lFor::RequireToken(token);
    shift();

    variable = std::make_unique<VariableDef>(this, token, shift);

    lIn::RequireToken(token);
    shift();

    range = std::make_unique<Expression>(this, token, shift);
    block = std::make_unique<Block>(this, token, shift);
}

void ForExpr::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "For: {\n";
    os << Indent(depth + 1) << "Symbols: " << Symbols() << "\n";
    variable ? variable->Print(os, depth + 1) : void();
    os << Indent(depth + 1) << "Range: {\n";
    range ? range->Print(os, depth + 2) : void();
    os << Indent(depth + 1) << "}\n";
    block ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "}\n";
}

VarID ForExpr::ReturnType() const {
    return block->ReturnType();
}

Return::Return(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lReturn::RequireToken(token);
    shift();

    if (!lSemicolon::MatchToken(token)) {
        value = std::make_unique<Expression>(this, token, shift);
    }

    lSemicolon::RequireToken(token);
    shift();
}

void Return::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Return: {\n";
    value ? value->Print(os, depth + 1) : void();
    os << Indent(depth) << "}\n";
}

VarID Return::ReturnType() const {
    return value ? value->ReturnType() : Symbols().GetSymbol("void").id;
}

Statement::Statement(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
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
        throw ParseException(token, Statement::ExpectedToken());
    }
}

void Statement::Print(std::ostream& os, size_t depth) const {
    std::visit(
        Visitor{
            [&, depth](const auto&) { os << Indent(depth) << "Unknown statement\n"; },
            [&, depth](const Return& arg) { arg.Print(os, depth); },
            [&, depth](const ForExpr& arg) { arg.Print(os, depth); },
            [&, depth](const IfExpr& arg) { arg.Print(os, depth); },
            [&, depth](const WhileExpr& arg) { arg.Print(os, depth); },
            [&, depth](const Expression& arg) { arg.Print(os, depth); },
        },
        expression
    );
}

VarID Statement::ReturnType() const {
    return std::visit(
        Visitor{
            [&](const auto&) -> VarID { throw; },
            [&](const Return& arg) { return arg.ReturnType(); },
            [&](const ForExpr& arg) { return arg.ReturnType(); },
            [&](const IfExpr& arg) { return arg.ReturnType(); },
            [&](const WhileExpr& arg) { return arg.ReturnType(); },
            [&](const Expression& arg) { return 0u; },
        },
        expression
    );
}

bool Statement::HasReturn() const {
    return std::visit(
        Visitor{
            [&](const auto&) -> bool { throw; },
            [&](const Return& arg) { return true; },
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
            [&](const Expression& arg) { return false; },
        },
        expression
    );
}

Block::Block(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lCurlyOpen::RequireToken(token);
    shift();

    while (!lCurlyClose::MatchToken(token)) {
        statements.emplace_back(this, token, shift);
    }

    lCurlyClose::RequireToken(token);
    shift();

    // Check return types
    auto anyType = Symbols().GetSymbol("any").id;
    returnType = 0;
    for (const auto& statement : statements) {
        std::visit(
            Visitor{
                [&](const auto&) { },
                [&](const Return& arg) { returnType = arg.ReturnType(); },
            },
            statement.expression
        );
        if (returnType != 0) break;
    }
    if (returnType == 0) {
        returnType = Symbols().GetSymbol("void").id;
    }

    // Validate other return types
    for (const auto& statement : statements) {
        auto current = statement.ReturnType();
        if (current == 0 || !statement.HasReturn()) continue;
        if (returnType != anyType && current != anyType && current != returnType) {
            throw TypeMismatchException(Symbols().GetName(returnType), Symbols().GetName(current));
        }

        if (returnType == anyType) {
            returnType = current;
        }
    }
}

void Block::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Block: {\n";
    for (const auto& statement : statements) {
        statement.Print(os, depth + 1);
    }
    os << Indent(depth) << "}\n";
}

VarID Block::ReturnType() const {
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

FunctionDef::FunctionDef(Node* parent, const std::string& signature, void* function)
    : Node(parent), symbols(&parent->Symbols()) {
    
    std::istringstream source(signature.c_str());
    Lexer lexer(source);
    auto token = lexer.Next();
    auto shift = [&]() { token = lexer.Next(); };

    lFunc::RequireToken(token);
    shift();

    lIdentifier::RequireToken(token);
    name = parent->Symbols().AddSymbol(token.text);
    shift();

    arguments = std::make_unique<Arguments>(this, token, shift);
    
}

FunctionDef::FunctionDef(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent), symbols(&parent->Symbols()) {
    lFunc::RequireToken(token);
    shift();

    lIdentifier::RequireToken(token);
    name = parent->Symbols().AddSymbol(token.text);
    shift();

    arguments = std::make_unique<Arguments>(this, token, shift);
    block = std::make_unique<Block>(this, token, shift);

    // Check return type
    auto anyType = Symbols().GetSymbol("any").id;
    if (block->ReturnType() == 0) {
        block->returnType = Symbols().GetSymbol("void").id;
    }
    if (ReturnType() == anyType) {
        arguments->returnType->typeName = block->ReturnType();
    } else if (ReturnType() != block->ReturnType() && block->ReturnType() != anyType) {
        throw TypeMismatchException(Symbols().GetName(ReturnType()), Symbols().GetName(block->ReturnType()));
    }

    parent->Symbols().SetType(name, ReturnType());
}

void FunctionDef::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "FunctionDef: {\n";
    os << Indent(depth + 1) << "Name: " << Symbols().GetName(name) << "\n";
    os << Indent(depth + 1) << "Symbols: " << Symbols() << "\n";
    arguments ? arguments->Print(os, depth + 1) : void();
    arguments ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "}\n";
}

VarID FunctionDef::ReturnType() const {
    return arguments->returnType->typeName;
}

Global::Global(const Token& token, const std::function<void()>& shift)
    : Node(nullptr), symbols(nullptr) {
    // Add predefined symbols and functions
    symbols.AddSymbol("any");
    symbols.AddSymbol("void");
    symbols.AddSymbol("bool");
    symbols.AddSymbol("string");
    symbols.AddSymbol("number");

    while (!lEoF::MatchToken(token)) {
        functions.emplace_back(this, token, shift);
    }
}

void Global::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Global: {\n";
    os << Indent(depth + 1) << "Symbols: " << Symbols() << "\n";
    for (const auto& func : functions)
        func.Print(os, depth + 1);
    os << Indent(depth) << "}\n";
}

} // namespace Interpreter
