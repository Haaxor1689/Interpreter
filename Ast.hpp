#pragma once

#include <variant>
#include <functional>

#include "Grammar.hpp"
#include "Rule.hpp"

namespace Interpreter {

using VarID = std::string;

// Literals
using lEoF = TokenType<Token::Type::EoF>;
using lFunc = TokenType<Token::Type::Func>;
using lIdentifier = TokenType<Token::Type::Identifier>;
using lParenOpen = TokenType<Token::Type::ParenOpen>;
using lParenClose = TokenType<Token::Type::ParenClose>;
using lCurlyOpen = TokenType<Token::Type::CurlyOpen>;
using lCurlyClose = TokenType<Token::Type::CurlyClose>;
using lSemicolon = TokenType<Token::Type::Semicolon>;
using lString = TokenType<Token::Type::String>;
using lNumber = TokenType<Token::Type::Number>;
using lFor = TokenType<Token::Type::For>;
using lIn = TokenType<Token::Type::In>;
using lIf = TokenType<Token::Type::If>;
using lElseif = TokenType<Token::Type::Elseif>;
using lElse = TokenType<Token::Type::Else>;
using lWhile = TokenType<Token::Type::While>;
using lDo = TokenType<Token::Type::Do>;
using lVar = TokenType<Token::Type::Var>;
using lComma = TokenType<Token::Type::Comma>;

struct Block;
struct Expression;

// Helper class for variant visiting
template<class... Ts>
struct Visitor : Ts... { using Ts::operator()...; };
template<class... Ts>
Visitor(Ts...) -> Visitor<Ts...>;

struct Node {
    Node* parent;
    virtual void Print(std::ostream& os, size_t depth) const = 0;
    std::string Indent(size_t depth) const {
        std::string ret;
        for (int i = 0; i < depth; ++i)
            ret += "    ";
        return ret;
    }
};

struct VariableDef : public Node, public Rule<lVar, lIdentifier> {
    VarID name;

    VariableDef(const Token& token, const std::function<void()>& shift) {
        lVar::RequireToken(token);
        shift();

        lIdentifier::RequireToken(token);
        name = token.text;
        shift();
    }

    void Print(std::ostream& os, size_t depth) const override {

    }
};

struct Arguments : public Node, public Rule<lParenOpen, List<Rule<VariableDef, lComma>>, lParenClose> {
    std::vector<VariableDef> arguments;

    Arguments(const Token& token, const std::function<void()>& shift) {
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

    void Print(std::ostream& os, size_t depth) const override {
        os << Indent(depth) << "Arguments: {\n";

        os << Indent(depth) << "}\n";
    }
};

struct FunctionCall : public Node, public Rule<lParenOpen, List<Rule<Expression, lComma>>, lParenClose> {
    VarID name;
    std::vector<Expression> arguments;

    FunctionCall(const Token& token, const std::function<void()>& shift) {
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

    void Print(std::ostream& os, size_t depth) const override {

    }
};

struct Expression : public Node, public RuleGroup<lIdentifier, lString, lNumber, VariableDef> {
    std::variant<std::monostate, FunctionCall, std::string, double, VariableDef> expression;

    Expression(const Token& token, const std::function<void()>& shift) {
        if (lIdentifier::MatchToken(token)) {
            lIdentifier::RequireToken(token);
            expression = token.text;
            shift();

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

    void Print(std::ostream& os, size_t depth) const override {
        std::visit(Visitor {
            [&, depth](auto) { os << Indent(depth) << "Unknown expression\n"; },
            [&, depth](const std::string& arg) { os << Indent(depth) << "String: " << arg << "\n"; },
            [&, depth](double arg) { os << Indent(depth) << "Double: " << arg << "\n"; },
            [&, depth](const Node& arg) { arg.Print(os, depth); },
        }, expression);
    }
};

struct WhileExpr : public Node, public RuleGroup<Rule<lWhile, Expression, Block>, Rule<lDo, Block, lWhile, Expression, lSemicolon>> {
    bool isDoWhile;
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Block> block;

    WhileExpr(const Token& token, const std::function<void()>& shift) {

    }

    void Print(std::ostream& os, size_t depth) const override {

    }
};

struct Else : public Node, public Rule<lElse, Block> {
    std::shared_ptr<Block> block;

    Else(const Token& token, const std::function<void()>& shift) {

    }

    void Print(std::ostream& os, size_t depth) const override {

    }
};

struct Elseif : public Node, public Rule<lElseif, Expression, Block> {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Block> block;

    Elseif(const Token& token, const std::function<void()>& shift) {

    }

    void Print(std::ostream& os, size_t depth) const override {

    }
};

struct If : public Node, public Rule<lIf, Expression, Block> {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Block> block;

    If(const Token& token, const std::function<void()>& shift) {

    }

    void Print(std::ostream& os, size_t depth) const override {

    }
};

struct IfExpr : public Node, public RuleGroup<If, Rule<If, List<Elseif>, Else>> {
    std::shared_ptr<If> ifStatement;
    std::vector<Elseif> elseifStatements;
    std::shared_ptr<Else> elseStatement;

    IfExpr(const Token& token, const std::function<void()>& shift) {

    }

    void Print(std::ostream& os, size_t depth) const override {

    }
};

struct ForExpr : public Node, public Rule<lFor, lIdentifier, lIn, Expression, Block> {
    VarID variable;
    std::shared_ptr<Expression> range;
    std::shared_ptr<Block> block;

    ForExpr(const Token& token, const std::function<void()>& shift) {
        lFor::RequireToken(token);
        shift();

        lIdentifier::RequireToken(token);
        variable = token.text;
        shift();

        lIn::RequireToken(token);
        shift();

        range = std::make_shared<Expression>(token, shift);

        lCurlyClose::RequireToken(token);
        shift();
    }

    void Print(std::ostream& os, size_t depth) const override {

    }
};

struct Statement : public Node, public RuleGroup<ForExpr, IfExpr, WhileExpr, Rule<Expression, lSemicolon>> {
    std::variant<std::monostate, ForExpr, IfExpr, WhileExpr, Expression> expression;

    Statement(const Token& token, const std::function<void()>& shift) {
        if (ForExpr::MatchToken(token)) {
            expression = ForExpr(token, shift);
        } else if (IfExpr::MatchToken(token)) {
            expression = IfExpr(token, shift);
        } else if (WhileExpr::MatchToken(token)) {
            expression = WhileExpr(token, shift);
        } else if (Expression::MatchToken(token)) {
            expression = Expression(token, shift);
        } else {
            throw ParseError(token, Statement::ExpectedToken());
        }

        lSemicolon::RequireToken(token);
        shift();
    }

    void Print(std::ostream& os, size_t depth) const override {
        std::visit(Visitor {
            [&, depth](auto) { os << Indent(depth) << "Unknown statement\n"; },
            [&, depth](const Expression& arg) { arg.Print(os, depth); },
        }, expression);
    }
};

struct Block : public Node, public Rule<lCurlyOpen, List<Statement>, lCurlyClose> {
    std::vector<Statement> statements;

    Block(const Token& token, const std::function<void()>& shift) {
        lCurlyOpen::RequireToken(token);
        shift();

        while (!lCurlyClose::MatchToken(token)) {
            statements.emplace_back(token, shift);
        }

        lCurlyClose::RequireToken(token);
        shift();
    }

    void Print(std::ostream& os, size_t depth) const override {
        os << Indent(depth) << "Block: {\n";
        for (const auto& statement : statements) {
            statement.Print(os, depth + 1);
        }
        os << Indent(depth) << "}\n";
    }
};

struct FunctionDef : public Node, public Rule<lFunc, lIdentifier, Arguments, Block> {
    VarID name;
    std::shared_ptr<Arguments> arguments;
    std::shared_ptr<Block> block;

    FunctionDef(const Token& token, const std::function<void()>& shift) {
        lFunc::RequireToken(token);
        shift();

        lIdentifier::RequireToken(token);
        name = token.text;
        shift();

        arguments = std::make_shared<Arguments>(token, shift);
        block = std::make_shared<Block>(token, shift);
    }

    void Print(std::ostream& os, size_t depth) const override {
        os << Indent(depth) << "FunctionDef: {\n";
        os << Indent(depth + 1) << "Name: " << name << "\n";
        if (arguments != nullptr)
            arguments->Print(os, depth + 1);
        if (block != nullptr)
            block->Print(os, depth + 1);
        os << Indent(depth) << "}\n";
    }
};

struct Global : public Node, public Rule<List<FunctionDef>, lEoF> {
    std::vector<FunctionDef> functions;

    Global(const Token& token, const std::function<void()>& shift) {
        while (!lEoF::MatchToken(token)) {
            functions.emplace_back(token, shift);
        }
    }

    void Print(std::ostream& os, size_t depth) const override {
        os << Indent(depth) << "Global: {\n";
        for (const auto& func : functions)
            func.Print(os, depth + 1);
        os << Indent(depth) << "}\n";
    }
};

struct Ast {
    std::shared_ptr<Global> root;

    Ast() = default;
    Ast(const Token& token, const std::function<void()>& shift) {
        root = std::make_shared<Global>(token, shift);
    }

    friend std::ostream& operator<<(std::ostream& os, const Ast& ast) {
        if (ast.root != nullptr)
            ast.root->Print(os, 0);
        return os;
    }
};
}