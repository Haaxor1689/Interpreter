#pragma once

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <string>
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
using lSquareOpen = TokenType<Token::Type::SquareOpen>;
using lSquareClose = TokenType<Token::Type::SquareClose>;
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
using lDot = TokenType<Token::Type::Dot>;

struct Block;
struct Expression;
struct ChainedOperation;
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
    virtual VarID ReturnType(const SymbolTable* scope = nullptr) const = 0;
    virtual void SetType(VarID type) { parent->SetType(type); };
    virtual SymbolTable& Symbols() { return parent->Symbols(); }
    virtual const SymbolTable& Symbols() const { return parent->Symbols(); }
    const SymbolTable& SymbolsOfType(const SymbolTable& scope, const VarRef& identifier) const;

    std::string Indent(size_t depth) const {
        std::string ret;
        for (size_t i = 0; i < depth; ++i)
            ret += "\t";
        return ret;
    }

    void MatchType(VarID symbol, VarID expectedType, const std::string& cause = "") {
        const auto anyType = Symbols()["any"].id;
        const auto& currentSymbol = Symbols()[symbol];
        if (currentSymbol.type == anyType) {
            Symbols().Set(symbol, expectedType, currentSymbol.isFunction, currentSymbol.isArray);
        }
        if (expectedType != anyType && currentSymbol.type != expectedType) {
            throw TypeMismatchException(ToString(Symbols()[currentSymbol.type]), ToString(Symbols()[expectedType]), line, cause);
        }
    }

    void CheckType(VarID symbolType, VarID expectedType, const std::string& cause = "") const {
        const auto anyType = Symbols()["any"].id;
        if (expectedType != anyType && symbolType != anyType && symbolType != expectedType) {
            throw TypeMismatchException(ToString(Symbols()[symbolType]), ToString(Symbols()[expectedType]), line, cause);
        }
    }
    
    Global& GetGlobal();
    const Global& GetGlobal() const;
};

struct IndexOperation : public Node, public Rule<lSquareOpen, Expression, lSquareClose> {
    VarRef identifier;
    std::unique_ptr<Expression> index;
    std::unique_ptr<ChainedOperation> chainedOperation;

    IndexOperation(const SymbolTable& scope, const VarRef& identifier, Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
    void SetType(VarID type) override;
};

struct DotOperation : public Node, public Rule<lDot, lIdentifier> {
    VarRef identifier;
    VarRef attribute;
    std::unique_ptr<ChainedOperation> chainedOperation;
    const SymbolTable& scope;

    DotOperation(const SymbolTable& scope, const VarRef& identifier, Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
    void SetType(VarID type) override;
};

struct Range : public Node, public Rule<Expression, lRangeOperator, Expression> {
    std::unique_ptr<Expression> from;
    std::unique_ptr<Expression> to;
    bool shouldIncludeLast;

    Range(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct UnaryOperation : public Node, public Rule<lUnaryOperator, Expression> {
    std::unique_ptr<Expression> value;
    std::string operation;
    VarID returnType;

    UnaryOperation(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct BinaryOperation : public Node, public Rule<lBinaryOperator, lParenOpen, Expression, lComma, Expression, lParenClose> {
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
    std::string operation;
    VarID returnType;

    BinaryOperation(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct VariableAssign : public Node, public Rule<lBinaryOperator, Expression> {
    VarRef identifier;
    std::unique_ptr<Expression> value;

    VariableAssign(const VarRef& identifier, Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct VariableRef : public Node, public Rule<lIdentifier> {
    VarID name;
    std::unique_ptr<ChainedOperation> chainedOperation;

    VariableRef(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
    void SetType(VarID type) override;
};

struct VariableDef : public Node, public Rule<lVar, lIdentifier> {
    VarID name;
    std::unique_ptr<Expression> value;
    bool isArray = false;

    VariableDef(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct ObjectInitializer : public Node, public Rule<lNew, lIdentifier, lCurlyOpen, List<VariableAssign>, lCurlyClose> {
    VarID type;
    std::map<VarID, Expression> values;

    ObjectInitializer(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override { return type; }
};

struct ArrayInitializer : public Node, public Rule<lSquareOpen, List<Expression>, lSquareClose> {
    VarID type;
    std::list<Expression> values;

    ArrayInitializer(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override { return type; }
};

struct Arguments : public Node, public Rule<lParenOpen, List<Rule<VariableDef, lComma>>, lParenClose> {
    std::list<VariableDef> arguments;

    Arguments(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override { return 0; }
};

struct FunctionCall : public Node, public Rule<lParenOpen, List<Rule<Expression, lComma>>, lParenClose> {
    VarRef identifier;
    std::list<Expression> arguments;
    std::unique_ptr<ChainedOperation> chainedOperation;

    FunctionCall(const SymbolTable& scope, const VarRef& identifier, Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct ChainedOperation : public Node, public RuleGroup<DotOperation, IndexOperation, VariableAssign, FunctionCall> {
    std::variant<std::monostate, DotOperation, IndexOperation, VariableAssign, FunctionCall> operation;

    ChainedOperation(const SymbolTable& scope, const VarRef& identifier, Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};


struct Expression : public Node, public RuleGroup<UnaryOperation, BinaryOperation, VariableRef, VariableDef, ObjectInitializer, ArrayInitializer, lTrue, lFalse, lNumber, lString> {
    std::variant<std::monostate, UnaryOperation, BinaryOperation, VariableRef, VariableDef, ObjectInitializer, ArrayInitializer, bool, double, std::string> expression;

    Expression(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
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
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct Else : public Node, public Rule<lElse, Block> {
    std::unique_ptr<Block> block;
    SymbolTable symbols;

    Else(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct Elseif : public Node, public Rule<lElseif, Expression, Block> {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Block> block;
    SymbolTable symbols;

    Elseif(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct If : public Node, public Rule<lIf, Expression, Block> {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Block> block;
    SymbolTable symbols;

    If(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct IfExpr : public Node, public Rule<If, List<Elseif>, Else> {
    std::unique_ptr<If> ifStatement;
    std::list<Elseif> elseifStatements;
    std::unique_ptr<Else> elseStatement;
    VarID returnType;

    IfExpr(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
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
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct Return : public Node, public Rule<lReturn, Expression> {
    std::unique_ptr<Expression> value;

    Return(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct Statement : public Node, public RuleGroup<Return, ForExpr, IfExpr, WhileExpr, Rule<Expression, lSemicolon>> {
    std::variant<std::monostate, Return, ForExpr, IfExpr, WhileExpr, Expression> expression;

    Statement(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
    bool HasReturn() const;
};

struct Block : public Node, public Rule<lCurlyOpen, List<Statement>, lCurlyClose> {
    std::list<Statement> statements;
    VarID returnType;

    Block(Node* parent, const Token& token, const std::function<void()>& shift);
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
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
    VarID ReturnType(const SymbolTable* scope = nullptr) const override;
};

struct ObjectDef : public Node, public Rule<lObject, lIdentifier, lCurlyOpen, List<VariableDef>, lCurlyClose> {
    VarID name;
    std::list<VariableDef> attributes;
    SymbolTable symbols;

    ObjectDef(Node* parent, const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override { return 0; }
};

struct Global : public Node, public Rule<List<RuleGroup<FunctionDef, ObjectDef>>, lEoF> {
    std::list<std::variant<FunctionDef, ObjectDef>> definitions;
    SymbolTable symbols;

    Global(const Token& token, const std::function<void()>& shift);
    SymbolTable& Symbols() override { return symbols; }
    const SymbolTable& Symbols() const override { return symbols; }
    void Print(std::ostream& os, size_t depth) const override;
    VarID ReturnType(const SymbolTable* scope = nullptr) const override { return 0; }

    FunctionDef& GetFunction(const VarRef& funcRef) {
        VarID funcId = std::visit(
            Visitor{
                [&](VarID arg) { return arg; },
                [&](const std::string& arg) { return symbols[arg].id; }
            },
            funcRef
        );
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
            throw UndefinedIdentifierException(funcId);
        }

        return std::get<FunctionDef>(*it);
    }

    const FunctionDef& GetFunction(const VarRef& funcRef) const {
        return const_cast<Global*>(this)->GetFunction(funcRef);
    }

    ObjectDef& GetObject(const VarRef& objRef) {
        VarID objId = std::visit(
            Visitor{
                [&](VarID arg) { return arg; },
                [&](const std::string& arg) { return symbols[arg].id; }
            },
            objRef
        );
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
            throw UndefinedIdentifierException(objId);
        }

        return std::get<ObjectDef>(*it);
    }

    const ObjectDef& GetObject(const VarRef& objRef) const {
        return const_cast<Global*>(this)->GetObject(objRef);
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
