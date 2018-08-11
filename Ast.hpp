#pragma once

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <variant>

#include "Helpers.hpp"
#include "Rule.hpp"
#include "SymbolTable.hpp"

namespace Interpreter {

// Literals
using lEoF = TokenType<Token::Type::EoF>;
using lFunc = TokenType<Token::Type::Func>;
using lIdentifier = TokenType<Token::Type::Identifier>;
using lParenOpen = TokenType<Token::Type::ParenOpen>;
using lParenClose = TokenType<Token::Type::ParenClose>;
using lCurlyOpen = TokenType<Token::Type::CurlyOpen>;
using lCurlyClose = TokenType<Token::Type::CurlyClose>;
using lColon = TokenType<Token::Type::Colon>;
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
using lReturn = TokenType<Token::Type::Return>;
using lTrue = TokenType<Token::Type::True>;
using lFalse = TokenType<Token::Type::False>;
using lUnaryOperator = TokenType<Token::Type::UnaryOperator>;
using lBinaryOperator = TokenType<Token::Type::BinaryOperator>;
using lRangeOperator = TokenType<Token::Type::RangeOperator>;
using lObject = TokenType<Token::Type::Object>;
using lNew = TokenType<Token::Type::New>;

struct Block;
struct Expression;
struct Global;

struct Node {
protected:
    Node(Node* parent, unsigned line)
        : parent(parent), line(line) {}

    Node(const Node&) = delete;
    Node(Node&&) = delete;
    Node& operator=(const Node&) = delete;
    Node& operator=(Node&&) = delete;

    virtual ~Node() {}

public:
    Node* parent;
    unsigned line;
    virtual void Print(std::ostream& os, size_t depth) const = 0;
    virtual VarID ReturnType() const = 0;
    virtual SymbolTable& Symbols() { return parent->Symbols(); }
    virtual const SymbolTable& Symbols() const { return parent->Symbols(); }
    std::string Indent(size_t depth) const {
        std::string ret;
        for (size_t i = 0; i < depth; ++i)
            ret += "    ";
        return ret;
    }
    void MatchType(VarID symbol, VarID expectedType, const std::string& cause = "") {
        const auto anyType = Symbols().GetSymbol("any").id;
        if (Symbols().GetType(symbol) == anyType) {
            Symbols().SetType(symbol, expectedType);
        }
        if (expectedType != anyType && Symbols().GetType(symbol) != expectedType) {
            throw TypeMismatchException(Symbols().GetName(Symbols().GetType(symbol)), Symbols().GetName(expectedType), line, cause);
        }
    }
    void MatchTypeConst(VarID symbol, VarID expectedType, const std::string& cause = "") const {
        const auto anyType = Symbols().GetSymbol("any").id;
        const auto symbolType = Symbols().GetType(symbol);
        if (expectedType != anyType && symbolType != anyType && symbolType != expectedType) {
            throw TypeMismatchException(Symbols().GetName(Symbols().GetType(symbol)), Symbols().GetName(expectedType), line, cause);
        }
    }
    
    Global& GetGlobal();
    const Global& GetGlobal() const;
};

struct Range : public Node, public Rule<Expression, lRangeOperator, Expression> {
    std::unique_ptr<Expression> from;
    std::unique_ptr<Expression> to;
    bool shouldIncludeLast;

    Range(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct UnaryOperation : public Node, public Rule<lUnaryOperator, Expression> {
    std::unique_ptr<Expression> value;
    std::string operation;
    VarID returnType;

    UnaryOperation(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct BinaryOperation : public Node, public Rule<lBinaryOperator, Expression, Expression> {
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
    std::string operation;
    VarID returnType;

    BinaryOperation(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct VariableAssign : public Node, public Rule<lBinaryOperator, Expression> {
    VarID name;
    std::unique_ptr<Expression> value;

    VariableAssign(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct VariableRef : public Node, public Rule<lIdentifier> {
    VarID name;

    VariableRef(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct VariableDef : public Node, public Rule<lVar, lIdentifier> {
    VarID name;
    std::unique_ptr<Expression> value;

    VariableDef(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct ObjectInitializer : public Node, public Rule<lNew, lIdentifier, lCurlyOpen, List<VariableAssign>, lCurlyClose> {
    VarID type;
    std::map<VarID, Expression> value;

    ObjectInitializer(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override { return type; }
};

struct Arguments : public Node, public Rule<lParenOpen, List<Rule<VariableDef, lComma>>, lParenClose> {
    std::list<VariableDef> arguments;

    Arguments(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override { return 0; }
};

struct FunctionCall : public Node, public Rule<lParenOpen, List<Rule<Expression, lComma>>, lParenClose> {
    VarID name;
    std::list<Expression> arguments;

    FunctionCall(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct Expression : public Node, public RuleGroup<BinaryOperation, VariableRef, lTrue, lFalse, lNumber, lString, VariableDef, ObjectInitializer> {
    std::variant<std::monostate, UnaryOperation, BinaryOperation, VariableRef, FunctionCall, VariableAssign, bool, double, std::string, VariableDef, ObjectInitializer> expression;

    Expression(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct WhileExpr : public Node, public RuleGroup<Rule<lWhile, Expression, Block>, Rule<lDo, Block, lWhile, Expression, lSemicolon>> {
    bool isDoWhile;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Block> block;
    SymbolTable symbols;

    WhileExpr(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct Else : public Node, public Rule<lElse, Block> {
    std::unique_ptr<Block> block;
    SymbolTable symbols;

    Else(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct Elseif : public Node, public Rule<lElseif, Expression, Block> {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Block> block;
    SymbolTable symbols;

    Elseif(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct If : public Node, public Rule<lIf, Expression, Block> {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Block> block;
    SymbolTable symbols;

    If(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct IfExpr : public Node, public Rule<If, List<Elseif>, Else> {
    std::unique_ptr<If> ifStatement;
    std::list<Elseif> elseifStatements;
    std::unique_ptr<Else> elseStatement;

    IfExpr(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct ForExpr : public Node, public Rule<lFor, VariableRef, lIn, Expression, Block> {
    std::unique_ptr<VariableDef> controlVariable;
    std::unique_ptr<Range> range;
    std::unique_ptr<Block> block;
    SymbolTable symbols;

    ForExpr(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct Return : public Node, public Rule<lReturn, Expression> {
    std::unique_ptr<Expression> value;

    Return(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct Statement : public Node, public RuleGroup<Return, ForExpr, IfExpr, WhileExpr, Rule<Expression, lSemicolon>> {
    std::variant<std::monostate, Return, ForExpr, IfExpr, WhileExpr, Expression> expression;

    Statement(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
    bool HasReturn() const;
};

struct Block : public Node, public Rule<lCurlyOpen, List<Statement>, lCurlyClose> {
    std::list<Statement> statements;
    VarID returnType;

    Block(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
    bool HasReturn() const;
};

struct ExternalFunction {
    using FunctionType = std::variant<std::monostate, void(*)(std::string), std::string(*)(), double(*)()>;
    FunctionType function;

    ExternalFunction(const FunctionType& function) : function(function) {};
};

struct FunctionDef : public Node, public Rule<lFunc, lIdentifier, Arguments, Block> {
    VarID name;
    std::unique_ptr<Arguments> arguments;
    std::unique_ptr<Block> block;
    SymbolTable symbols;
    
    ExtFunctionType externalFunction;

    FunctionDef(Node* parent, const std::string& signature, ExtFunctionType&& function);
    FunctionDef(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override;
};

struct ObjectDef : public Node, public Rule<lObject, lIdentifier, lCurlyOpen, List<VariableDef>, lCurlyClose> {
    VarID name;
    std::list<VariableDef> attributes;
    SymbolTable symbols;

    ObjectDef(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override { return 0; }
};

struct Global : public Node, public Rule<List<RuleGroup<FunctionDef, ObjectDef>>, lEoF> {
    std::list<std::variant<FunctionDef, ObjectDef>> definitions;
    SymbolTable symbols;

    Global(const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType() const override { return 0; }

    const FunctionDef& GetFunction(const std::string& name) const {
        return GetFunction(symbols.GetSymbol(name).id);
    }

    const FunctionDef& GetFunction(VarID funcId) const {
        auto it = std::find_if(definitions.begin(), definitions.end(), [&](const auto& val) {
            return std::visit(
                Visitor{
                    [&](const auto&) { return false; },
                    [&](const FunctionDef& arg) { return arg.name == funcId; },
                },
                val
            );
        });
        if (it == definitions.end()) {
            throw UndefinedIdentifierNameException(funcId);
        }

        return std::get<FunctionDef>(*it);
    }

    const ObjectDef& GetObject(const std::string& name) const {
        return GetObject(symbols.GetSymbol(name).id);
    }

    const ObjectDef& GetObject(VarID objId) const {
        auto it = std::find_if(definitions.begin(), definitions.end(), [&](const auto& val) {
            return std::visit(
                Visitor{
                    [&](const auto&) { return false; },
                    [&](const ObjectDef& arg) { return arg.name == objId; },
                },
                val
            );
        });
        if (it == definitions.end()) {
            throw UndefinedIdentifierNameException(objId);
        }

        return std::get<ObjectDef>(*it);
    }
};

class Ast {
    Global root;

public:
    Ast(const Token& token, const std::function<void()>& shift)
        : root(token, shift) {}

    const Global& Root() const { return root; }

    friend std::ostream& operator<<(std::ostream& os, const Ast& ast) {
        ast.root.Print(os, 0);
        return os;
    }
};

} // namespace Interpreter
