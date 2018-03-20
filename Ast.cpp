#include "Ast.hpp"

namespace Interpreter {

VariableRef::VariableRef(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lIdentifier::RequireToken(token);
    name = GetSymbols().GetSymbol(token.text);
    shift();
}

void VariableRef::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Variable: " << name << "\n";
}

VariableDef::VariableDef(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lVar::RequireToken(token);
    shift();

    lIdentifier::RequireToken(token);
    name = GetSymbols().AddSymbol(token.text);
    shift();
}

void VariableDef::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Variable: " << name << "\n";
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
}

void Arguments::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Arguments: {\n";
    for (const auto& arg : arguments)
        arg.Print(os, depth + 1);
    os << Indent(depth) << "}\n";
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
    os << Indent(depth + 1) << "Name: " << name << "\n";
    os << Indent(depth + 1) << "Arguments: {\n";
    for (const auto& arg : arguments)
        arg.Print(os, depth + 2);
    os << Indent(depth + 1) << "}\n";
    os << Indent(depth) << "}\n";
}

Expression::Expression(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    if (lIdentifier::MatchToken(token)) {
        lIdentifier::RequireToken(token);
        expression = VariableRef(this, token, shift);

        if (FunctionCall::MatchToken(token)) {
            VarID name = std::get<VariableRef>(expression).name;
            FunctionCall fc(this, token, shift);
            fc.name = name;
            expression = std::move(fc);
        }
    } else if (lString::MatchToken(token)) {
        lString::RequireToken(token);
        expression = token.text;
        shift();
    } else if (lNumber::MatchToken(token)) {
        lNumber::RequireToken(token);
        expression = std::stod(token.text);
        shift();
    } else if (VariableDef::MatchToken(token)) {
        expression = VariableDef(this, token, shift);
    } else {
        throw ParseError(token, ExpectedToken());
    }
}

void Expression::Print(std::ostream& os, size_t depth) const {
    std::visit(
        Visitor{
            [&, depth](auto) { os << Indent(depth) << "Unknown expression\n"; },
            [&, depth](const VariableRef& arg) { arg.Print(os, depth); },
            [&, depth](const FunctionCall& arg) { arg.Print(os, depth); },
            [&, depth](const std::string& arg) { os << Indent(depth) << "String: " << arg << "\n"; },
            [&, depth](double arg) { os << Indent(depth) << "Double: " << arg << "\n"; },
            [&, depth](const VariableDef& arg) { arg.Print(os, depth); },
        },
        expression);
}

WhileExpr::WhileExpr(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {}

void WhileExpr::Print(std::ostream& os, size_t depth) const {}

Else::Else(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {}

void Else::Print(std::ostream& os, size_t depth) const {}

Elseif::Elseif(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {}

void Elseif::Print(std::ostream& os, size_t depth) const {}

If::If(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {}

void If::Print(std::ostream& os, size_t depth) const {}

IfExpr::IfExpr(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {}

void IfExpr::Print(std::ostream& os, size_t depth) const {}

ForExpr::ForExpr(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    lFor::RequireToken(token);
    shift();

    variable = std::make_shared<VariableRef>(this, token, shift);

    lIn::RequireToken(token);
    shift();

    range = std::make_shared<Expression>(this, token, shift);
    block = std::make_shared<Block>(this, token, shift);
}

void ForExpr::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "For: {\n";
    if (variable != nullptr)
        variable->Print(os, depth + 1);
    os << Indent(depth + 1) << "Range: {\n";
    if (range != nullptr)
        range->Print(os, depth + 2);
    os << Indent(depth + 1) << "}\n";
    if (block != nullptr)
        block->Print(os, depth + 1);
    os << Indent(depth) << "}\n";
}

Statement::Statement(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent) {
    if (ForExpr::MatchToken(token)) {
        expression = ForExpr(this, token, shift);
    } else if (IfExpr::MatchToken(token)) {
        expression = IfExpr(this, token, shift);
    } else if (WhileExpr::MatchToken(token)) {
        expression = WhileExpr(this, token, shift);
    } else if (Expression::MatchToken(token)) {
        expression = Expression(this, token, shift);

        lSemicolon::RequireToken(token);
        shift();
    } else {
        throw ParseError(token, Statement::ExpectedToken());
    }
}

void Statement::Print(std::ostream& os, size_t depth) const {
    std::visit(
        Visitor{
            [&, depth](auto) { os << Indent(depth) << "Unknown statement\n"; },
            [&, depth](const ForExpr& arg) { arg.Print(os, depth); },
            [&, depth](const IfExpr& arg) { arg.Print(os, depth); },
            [&, depth](const WhileExpr& arg) { arg.Print(os, depth); },
            [&, depth](const Expression& arg) { arg.Print(os, depth); },
        },
        expression);
}

Block::Block(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent), symbols(parent->GetSymbols()) {
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

FunctionDef::FunctionDef(Node* parent, const Token& token, const std::function<void()>& shift)
    : Node(parent), symbols(parent->GetSymbols()) {
    lFunc::RequireToken(token);
    shift();

    lIdentifier::RequireToken(token);
    name = GetSymbols().AddSymbol(token.text);
    shift();

    arguments = std::make_shared<Arguments>(this, token, shift);
    block = std::make_shared<Block>(this, token, shift);
}

void FunctionDef::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "FunctionDef: {\n";
    os << Indent(depth + 1) << "Name: " << name << "\n";
    if (arguments != nullptr)
        arguments->Print(os, depth + 1);
    if (block != nullptr)
        block->Print(os, depth + 1);
    os << Indent(depth) << "}\n";
}

Global::Global(const Token& token, const std::function<void()>& shift)
    : Node(nullptr), symbols(nullptr) {
    while (!lEoF::MatchToken(token)) {
        functions.emplace_back(this, token, shift);
    }
}

void Global::Print(std::ostream& os, size_t depth) const {
    os << Indent(depth) << "Global: {\n";
    for (const auto& func : functions)
        func.Print(os, depth + 1);
    os << Indent(depth) << "}\n";
}

} // namespace Interpreter
