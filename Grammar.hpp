#pragma once

#include "Rule.hpp"
#include "Token.hpp"

namespace Interpreter {
namespace Grammar {

// Literals
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

// Forward declarations
struct Global;
struct FunctionDef;
struct Block;
struct Statement;
struct ForExpr;
struct IfExpr;
struct If;
struct Elseif;
struct Else;
struct WhileExpr;
struct Expression;
struct FunctionCall;
struct Arguments;

// #define RULE(name, ...) struct name : public Rule<__VA_ARGS__> { using base = Rule<__VA_ARGS__>; using base::base; };
// #define RULE_GROUP(name, ...) struct name : public RuleGroup<__VA_ARGS__> { using base = RuleGroup<__VA_ARGS__>; using base::base; };

// Rules
// RULE(Arguments, lParenOpen, lIdentifier, lParenClose)
struct Arguments : public Rule<lParenOpen, lIdentifier, lParenClose> {};
// RULE(FunctionCall, lIdentifier, Arguments)
struct FunctionCall : public Rule<lIdentifier, Arguments> {};
// RULE(Expression1, FunctionCall)
// RULE(Expression2, lIdentifier)
// RULE(Expression3, lString)
// RULE(Expression4, lNumber)
// RULE_GROUP(Expression, Expression1, Expression2, Expression3, Expression4)
struct Expression : public RuleGroup<FunctionCall, lIdentifier, lString, lNumber> {};
// RULE(WhileExpr1, lWhile, Expression, Block*)
// RULE(WhileExpr2, lDo, Block*, lWhile, Expression, lSemicolon)
// RULE_GROUP(WhileExpr, WhileExpr1, WhileExpr2)
struct WhileExpr : public RuleGroup<Rule<lWhile, Expression, Block*>, Rule<lDo, Block*, lWhile, Expression, lSemicolon>> {};
// RULE(Else, lElse, Block*)
struct Else : Rule<lElse, Block*> {};
// RULE(Elseif, lElseif, Expression, Block*)
struct Elseif : Rule<lElseif, Expression, Block*> {};
// RULE(If, lIf, Expression, Block*)
struct If : public Rule<lIf, Expression, Block*> {};
// RULE(IfExpr1, If)
// RULE(IfExpr2, If, List<Elseif>, Else)
// RULE_GROUP(IfExpr, IfExpr1, IfExpr2)
struct IfExpre : public RuleGroup<If, Rule<If, List<Elseif>, Else>> {};
// RULE(ForExpr, lIdentifier, lIn, Expression, Block*)
struct ForExpr : public Rule<lIdentifier, lIn, Expression, Block*> {};
// RULE(Statement1, ForExpr)
// RULE(Statement2, IfExpr)
// RULE(Statement3, WhileExpr)
// RULE(Statement4, Expression, lSemicolon)
// RULE_GROUP(Statement, Statement1, Statement2, Statement3, Statement4)
struct Statement : RuleGroup<ForExpr, IfExpr, WhileExpr, Rule<Expression, lSemicolon>> {};
// RULE(Block, lCurlyOpen, List<Statement>, lCurlyClose)
struct Block : public Rule<lCurlyOpen, List<Statement>, lCurlyClose> {};
// RULE(FunctionDef, lFunc, lIdentifier, Arguments, Block)
struct FunctionDef : public Rule<lFunc, lIdentifier, Arguments, Block> {};
struct Global : public List<FunctionDef> {};
// RULE(Global, List<FunctionDef>)
}
}