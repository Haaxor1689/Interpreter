#pragma once

#include <variant>
#include <memory>
#include <functional>
#include <list>

#include "Grammar.hpp"
#include "Rule.hpp"
#include "SymbolTable.hpp"

namespace Interpreter {

using VarID = unsigned;

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
template <class... Ts>
struct Visitor : Ts... { using Ts::operator()...; };
template <class... Ts>
Visitor(Ts...)->Visitor<Ts...>;

struct Node {
protected:
    Node* parent;
    Node(Node* parent)
        : parent(parent) {}
    virtual ~Node() {}

public:
    virtual void Print(std::ostream& os, size_t depth) const = 0;
    virtual SymbolTable& Symbols() { return parent->Symbols(); }
    virtual const SymbolTable& Symbols() const { return parent->Symbols(); }
    std::string Indent(size_t depth) const {
        std::string ret;
        for (size_t i = 0; i < depth; ++i)
            ret += "    ";
        return ret;
    }
};

class VariableRef : public Node, public Rule<lIdentifier> {
public:
    VarID name;

    VariableRef(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
};

class VariableDef : public Node, public Rule<lVar, lIdentifier> {
public:
    VarID name;

    VariableDef(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
};

class Arguments : public Node, public Rule<lParenOpen, List<Rule<VariableDef, lComma>>, lParenClose> {
    std::list<VariableDef> arguments;

public:
    Arguments(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
};

class FunctionCall : public Node, public Rule<lParenOpen, List<Rule<Expression, lComma>>, lParenClose> {
    std::list<Expression> arguments;

public:
    VarID name;

    FunctionCall(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
};

class Expression : public Node, public RuleGroup<lIdentifier, lString, lNumber, VariableDef> {
    std::variant<std::monostate, VariableRef, FunctionCall, std::string, double, VariableDef> expression;

public:
    Expression(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
};

class WhileExpr : public Node,
                  public RuleGroup<Rule<lWhile, Expression, Block>, Rule<lDo, Block, lWhile, Expression, lSemicolon>> {
    bool isDoWhile;
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Block> block;

public:
    WhileExpr(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
};

class Else : public Node, public Rule<lElse, Block> {
    std::shared_ptr<Block> block;

public:
    Else(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
};

class Elseif : public Node, public Rule<lElseif, Expression, Block> {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Block> block;

public:
    Elseif(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
};

class If : public Node, public Rule<lIf, Expression, Block> {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Block> block;

public:
    If(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
};

class IfExpr : public Node, public Rule<If, List<Elseif>, Else> {
    std::shared_ptr<If> ifStatement;
    std::list<Elseif> elseifStatements;
    std::shared_ptr<Else> elseStatement;

public:
    IfExpr(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
};

class ForExpr : public Node, public Rule<lFor, VariableRef, lIn, Expression, Block> {
    std::shared_ptr<VariableDef> variable;
    std::shared_ptr<Expression> range;
    std::shared_ptr<Block> block;

public:
    ForExpr(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
};

class Statement : public Node, public RuleGroup<ForExpr, IfExpr, WhileExpr, Rule<Expression, lSemicolon>> {
    std::variant<std::monostate, ForExpr, IfExpr, WhileExpr, Expression> expression;

public:
    Statement(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
};

class Block : public Node, public Rule<lCurlyOpen, List<Statement>, lCurlyClose> {
    std::list<Statement> statements;
    SymbolTable symbols;

public:
    Block(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
};

class FunctionDef : public Node, public Rule<lFunc, lIdentifier, Arguments, Block> {
    std::shared_ptr<Arguments> arguments;
    std::shared_ptr<Block> block;
    SymbolTable symbols;

public:
    VarID name;

    FunctionDef(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
};

class Global : public Node, public Rule<List<FunctionDef>, lEoF> {
    std::list<FunctionDef> functions;
    SymbolTable symbols;

public:
    Global(const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
};

class Ast {
    std::shared_ptr<Global> root;

public:
    Ast() = default;
    Ast(const Token& token, const std::function<void()>& shift) {
        root = std::make_shared<Global>(token, shift);
    }

    const Global& Root() const { return *root; }

    friend std::ostream& operator<<(std::ostream& os, const Ast& ast) {
        if (ast.root != nullptr)
            ast.root->Print(os, 0);
        return os;
    }
};

} // namespace Interpreter
