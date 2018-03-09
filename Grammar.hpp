#pragma once

#include "Rule.hpp"
#include "Token.hpp"

namespace Interpreter {
namespace Grammar {

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

// Forward declarations
struct Block;

// Rules
struct Arguments : public Rule<lParenOpen, List<lIdentifier>, lParenClose> {};
struct FunctionCall : public Rule<Arguments> {};
struct IdentifierExp : public RuleGroup<FunctionCall, Epsilon> {};
struct VariableDef : public Rule<lVar, lIdentifier> {};
struct Expression : public RuleGroup<Rule<lIdentifier, IdentifierExp>, lString, lNumber, VariableDef> {};
struct WhileExpr : public RuleGroup<Rule<lWhile, Expression, Block*>, Rule<lDo, Block, lWhile, Expression, lSemicolon>> {};
struct Else : Rule<lElse, Block> {};
struct Elseif : Rule<lElseif, Expression, Block> {};
struct If : public Rule<lIf, Expression, Block> {};
struct IfExpr : public RuleGroup<If, Rule<If, List<Elseif>, Else>> {};
struct ForExpr : public Rule<lFor, lIdentifier, lIn, Expression, Block> {};
struct Statement : RuleGroup<ForExpr, IfExpr, WhileExpr, Rule<Expression, lSemicolon>> {};
struct Block : public Rule<lCurlyOpen, List<Statement>, lCurlyClose> {};
struct FunctionDef : public Rule<lFunc, lIdentifier, Arguments, Block> {};
struct Global : public Rule<List<FunctionDef>, lEoF> {};

}
}