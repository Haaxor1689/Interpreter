#include "Ast.hpp"

namespace Interpreter {

VariableRef::VariableRef(const Token &token, const std::function<void ()> &shift) {
    lIdentifier::RequireToken(token);
    name = token.text;
    shift();
}

void VariableRef::Print(std::ostream &os, size_t depth) const {
    os << Indent(depth) << "Variable: " << name << "\n";
}

VariableDef::VariableDef(const Token &token, const std::function<void ()> &shift) {
    lVar::RequireToken(token);
    shift();

    lIdentifier::RequireToken(token);
    name = token.text;
    shift();
}

void VariableDef::Print(std::ostream &os, size_t depth) const {
    os << Indent(depth) << "Variable: " << name << "\n";
}

Arguments::Arguments(const Token &token, const std::function<void ()> &shift) {
    lParenOpen::RequireToken(token);
    shift();

    if (!lParenClose::MatchToken(token))
        while (true) {
            arguments.emplace_back(token, shift);

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

void Arguments::Print(std::ostream &os, size_t depth) const {
    os << Indent(depth) << "Arguments: {\n";
    for (const auto& arg : arguments)
        arg.Print(os, depth + 1);
    os << Indent(depth) << "}\n";
}

FunctionCall::FunctionCall(const Token &token, const std::function<void ()> &shift) {
    lParenOpen::RequireToken(token);
    shift();

    if (!lParenClose::MatchToken(token))
        while (true) {
            arguments.emplace_back(token, shift);

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

void FunctionCall::Print(std::ostream &os, size_t depth) const {

}

Expression::Expression(const Token &token, const std::function<void ()> &shift) {
    if (lIdentifier::MatchToken(token)) {
        lIdentifier::RequireToken(token);
        expression = VariableRef(token, shift);

        if (FunctionCall::MatchToken(token)) {
            std::string name = std::get<std::string>(expression);
            FunctionCall fc(token, shift);
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
        expression = VariableDef(token, shift);
    } else {
        throw ParseError(token, ExpectedToken());
    }
}

void Expression::Print(std::ostream &os, size_t depth) const {
    std::visit(Visitor {
                   [&, depth](auto) { os << Indent(depth) << "Unknown expression\n"; },
                   [&, depth](const VariableRef& arg) { arg.Print(os, depth); },
                   [&, depth](const FunctionCall& arg) { arg.Print(os, depth); },
                   [&, depth](const std::string& arg) { os << Indent(depth) << "String: " << arg << "\n"; },
                   [&, depth](double arg) { os << Indent(depth) << "Double: " << arg << "\n"; },
                   [&, depth](const VariableDef& arg) { arg.Print(os, depth); },
               }, expression);
}

WhileExpr::WhileExpr(const Token &token, const std::function<void ()> &shift) {

}

void WhileExpr::Print(std::ostream &os, size_t depth) const {

}

Else::Else(const Token &token, const std::function<void ()> &shift) {

}

void Else::Print(std::ostream &os, size_t depth) const {

}

Elseif::Elseif(const Token &token, const std::function<void ()> &shift) {

}

void Elseif::Print(std::ostream &os, size_t depth) const {

}

If::If(const Token &token, const std::function<void ()> &shift) {

}

void If::Print(std::ostream &os, size_t depth) const {

}

IfExpr::IfExpr(const Token &token, const std::function<void ()> &shift) {

}

void IfExpr::Print(std::ostream &os, size_t depth) const {

}

ForExpr::ForExpr(const Token &token, const std::function<void ()> &shift) {
    lFor::RequireToken(token);
    shift();

    variable = std::make_shared<VariableRef>(token, shift);

    lIn::RequireToken(token);
    shift();

    range = std::make_shared<Expression>(token, shift);
    block = std::make_shared<Block>(token, shift);
}

void ForExpr::Print(std::ostream &os, size_t depth) const {
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

Statement::Statement(const Token &token, const std::function<void ()> &shift) {
    if (ForExpr::MatchToken(token)) {
        expression = ForExpr(token, shift);
    } else if (IfExpr::MatchToken(token)) {
        expression = IfExpr(token, shift);
    } else if (WhileExpr::MatchToken(token)) {
        expression = WhileExpr(token, shift);
    } else if (Expression::MatchToken(token)) {
        expression = Expression(token, shift);

        lSemicolon::RequireToken(token);
        shift();
    } else {
        throw ParseError(token, Statement::ExpectedToken());
    }
}

void Statement::Print(std::ostream &os, size_t depth) const {
    std::visit(Visitor {
                   [&, depth](auto) { os << Indent(depth) << "Unknown statement\n"; },
                   [&, depth](const ForExpr& arg) { arg.Print(os, depth); },
                   [&, depth](const IfExpr& arg) { arg.Print(os, depth); },
                   [&, depth](const WhileExpr& arg) { arg.Print(os, depth); },
                   [&, depth](const Expression& arg) { arg.Print(os, depth); },
               }, expression);
}

Block::Block(const Token &token, const std::function<void ()> &shift) {
    lCurlyOpen::RequireToken(token);
    shift();

    while (!lCurlyClose::MatchToken(token)) {
        statements.emplace_back(token, shift);
    }

    lCurlyClose::RequireToken(token);
    shift();
}

void Block::Print(std::ostream &os, size_t depth) const {
    os << Indent(depth) << "Block: {\n";
    for (const auto& statement : statements) {
        statement.Print(os, depth + 1);
    }
    os << Indent(depth) << "}\n";
}

FunctionDef::FunctionDef(const Token &token, const std::function<void ()> &shift) {
    lFunc::RequireToken(token);
    shift();

    lIdentifier::RequireToken(token);
    name = token.text;
    shift();

    arguments = std::make_shared<Arguments>(token, shift);
    block = std::make_shared<Block>(token, shift);
}

void FunctionDef::Print(std::ostream &os, size_t depth) const {
    os << Indent(depth) << "FunctionDef: {\n";
    os << Indent(depth + 1) << "Name: " << name << "\n";
    if (arguments != nullptr)
        arguments->Print(os, depth + 1);
    if (block != nullptr)
        block->Print(os, depth + 1);
    os << Indent(depth) << "}\n";
}

Global::Global(const Token &token, const std::function<void ()> &shift) {
    while (!lEoF::MatchToken(token)) {
        functions.emplace_back(token, shift);
    }
}

void Global::Print(std::ostream &os, size_t depth) const {
    os << Indent(depth) << "Global: {\n";
    for (const auto& func : functions)
        func.Print(os, depth + 1);
    os << Indent(depth) << "}\n";
}

}
