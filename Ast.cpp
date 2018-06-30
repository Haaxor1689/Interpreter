#include "Ast.hpp"

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

VariableAssign::VariableAssign(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lBinaryOperator::RequireToken(token);
    shift();

    value = std::make_unique<Expression>(this, token, shift);
}

void VariableAssign::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Assignment: {\n";
    os << Indent(depth + 1) << "Variable: " << Symbols().GetName(name) << "\n";
    os << Indent(depth + 1) << "Value: {\n";
    value->Print(os, depth + 2);
    os << Indent(depth + 1) << "}\n";
    os << Indent(depth) << "}\n";
}

TypeName::TypeName(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lColon::RequireToken(token);
    shift();
        
    lIdentifier::RequireToken(token);
    typeName = Symbols().GetSymbol(token.text);
    shift();
}

void TypeName::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Type: " << Symbols().GetName(typeName) << "\n";
}

VariableRef::VariableRef(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lIdentifier::RequireToken(token);
    name = Symbols().GetSymbol(token.text);
    shift();
}

void VariableRef::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Variable: " << Symbols().GetName(name) << "\n";
}

VariableDef::VariableDef(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lVar::RequireToken(token);
    shift();

    lIdentifier::RequireToken(token);
    name = Symbols().AddSymbol(token.text);
    shift();

    if (lColon::MatchToken(token)) {
        type = std::make_unique<TypeName>(this, token, shift);
    }

    if (lBinaryOperator::MatchToken(token) && token.text == "=") {
        shift();

        value = std::make_unique<Expression>(this, token, shift);
    }
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
    
    if (lColon::MatchToken(token)) {
        returnType = std::make_unique<TypeName>(this, token, shift);
    }
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

Return::Return(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lReturn::RequireToken(token);
    shift();

    value = std::make_unique<Expression>(this, token, shift);

    lSemicolon::RequireToken(token);
    shift();
}

void Return::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Return: {\n";
    value ? value->Print(os, depth + 1) : void();
    os << Indent(depth) << "}\n";
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

Block::Block(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lCurlyOpen::RequireToken(token);
    shift();

    while (!lCurlyClose::MatchToken(token)) {
        statements.emplace_back(this, token, shift);
    }

    lCurlyClose::RequireToken(token);
    shift();
}

void Block::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Block: {\n";
    for (const auto& statement : statements) {
        statement.Print(os, depth + 1);
    }
    os << Indent(depth) << "}\n";
}

bool Block::HasReturnStatement() const {
    bool hasReturn = false;
    for (const auto& statement : statements) {
        std::visit(
            Visitor{
                [&](const auto&) { },
                [&](const Return& arg) { hasReturn = true; },
                [&](const ForExpr& arg) { hasReturn = arg.block->HasReturnStatement(); },
                [&](const IfExpr& arg) {
                    hasReturn = arg.ifStatement->block->HasReturnStatement();
                    if (hasReturn) return;

                    for (const auto& elseif : arg.elseifStatements) {
                        hasReturn = elseif.block->HasReturnStatement();
                        if (hasReturn) return;
                    }

                    if (arg.elseStatement) {
                        hasReturn = arg.elseStatement->block->HasReturnStatement();
                    }
                },
                [&](const WhileExpr& arg) { hasReturn = arg.block->HasReturnStatement(); },
            },
            statement.expression
        );
        if (hasReturn) {
            return true;
        }
    }
    return false;
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

    auto voidId = Symbols().GetSymbol("void");
    if (arguments->returnType && arguments->returnType->typeName != voidId && !block->HasReturnStatement()) {
        throw TypeMismatchException(Symbols().GetName(arguments->returnType->typeName), "void");
    }
}

void FunctionDef::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "FunctionDef: {\n";
    os << Indent(depth + 1) << "Name: " << Symbols().GetName(name) << "\n";
    os << Indent(depth + 1) << "Symbols: " << Symbols() << "\n";
    arguments ? arguments->Print(os, depth + 1) : void();
    arguments ? block->Print(os, depth + 1) : void();
    os << Indent(depth) << "}\n";
}

Global::Global(const Token& token, const std::function<void()>& shift)
    : Node(nullptr), symbols(nullptr) {
    // Add predefined symbols and functions
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
