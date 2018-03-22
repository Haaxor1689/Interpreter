#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <sstream>

#include "Interpreter"

using namespace std;
using namespace Interpreter;

namespace {

vector<Token> GetTokens(Lexer& lexer) {
    vector<Token> tokens;
    while (true) {
        tokens.push_back(lexer.Next());
        if (tokens.back().type == Token::Type::EoF)
            return tokens;
    }
}

std::string PrintTokens(const vector<Token>& tokens) {
    ostringstream oss;
    for (const auto& token : tokens)
        oss << token << endl;
    return oss.str();
}

void TryCreateParser(const std::string& name) {
    Parser p("examples/" + name);
}

} // namespace

TEST_CASE("Lexer") {
    SECTION("Empty file gives EoF token") {
        Lexer lexer("examples/lexer/LexerEmpty.ct");
        auto tokens = GetTokens(lexer);
        INFO(PrintTokens(tokens));
        REQUIRE(tokens.size() == 1);
        CHECK(tokens[0] == Token("", Token::Type::EoF, 1));
    }

    SECTION("File with only a comment") {
        Lexer lexer("examples/lexer/LexerComment.ct");
        auto tokens = GetTokens(lexer);
        INFO(PrintTokens(tokens));
        REQUIRE(tokens.size() == 1);
        CHECK(tokens[0] == Token("", Token::Type::EoF, 2));
    }

    SECTION("Two identifiers split by space") {
        Lexer lexer("examples/lexer/LexerTwoIdentifiers.ct");
        auto tokens = GetTokens(lexer);
        INFO(PrintTokens(tokens));
        REQUIRE(tokens.size() == 3);
        CHECK(tokens[0] == Token("foo", Token::Type::Identifier, 2));
        CHECK(tokens[0] == Token("foo", Token::Type::Identifier, 2));
        CHECK(tokens[2] == Token("", Token::Type::EoF, 3));
    }

    SECTION("Number tokens") {
        Lexer lexer("examples/lexer/LexerNumbers.ct");
        auto tokens = GetTokens(lexer);
        INFO(PrintTokens(tokens));
        REQUIRE(tokens.size() == 4);
        CHECK(tokens[0] == Token("foo2", Token::Type::Identifier, 2));
        CHECK(tokens[1] == Token("2.1", Token::Type::Number, 2));
        CHECK(tokens[2] == Token("as", Token::Type::As, 2));
    }

    SECTION("String tokens") {
        Lexer lexer("examples/lexer/LexerStrings.ct");
        auto tokens = GetTokens(lexer);
        INFO(PrintTokens(tokens));
        REQUIRE(tokens.size() == 6);
        CHECK(tokens[0] == Token("foo", Token::Type::Identifier, 2));
        CHECK(tokens[1] == Token("\"\"", Token::Type::String, 2));
        CHECK(tokens[2] == Token("\"foo 1\"", Token::Type::String, 2));
        CHECK(tokens[3] == Token("3", Token::Type::Number, 2));
        CHECK(tokens[4] == Token("goo2", Token::Type::Identifier, 2));
    }

    SECTION("Invalid string tokens") {
        Lexer lexer("examples/lexer/LexerInvalidString.ct");
        auto tokens = GetTokens(lexer);
        INFO(PrintTokens(tokens));
        REQUIRE(tokens.size() == 5);
        CHECK(tokens[0] == Token("foo", Token::Type::Identifier, 2));
        CHECK(tokens[1] == Token("\"2\"", Token::Type::String, 2));
        CHECK(tokens[2] == Token("\"123", Token::Type::Invalid, 2));
        CHECK(tokens[3] == Token("goo", Token::Type::Identifier, 3));
        CHECK(tokens[4] == Token("", Token::Type::EoF, 4));
    }

    SECTION("Operators") {
        Lexer lexer("examples/lexer/LexerOperators.ct");
        auto tokens = GetTokens(lexer);
        INFO(PrintTokens(tokens));
        REQUIRE(tokens.size() == 19);
        CHECK(tokens[1] == Token("+=", Token::Type::BinaryOperator, 2));
        CHECK(tokens[4] == Token("!", Token::Type::UnaryOperator, 3));
        CHECK(tokens[6] == Token("!=", Token::Type::BinaryOperator, 4));
        CHECK(tokens[9] == Token(".", Token::Type::BinaryOperator, 5));
        CHECK(tokens[12] == Token("->", Token::Type::BinaryOperator, 6));
        CHECK(tokens[15] == Token("*|", Token::Type::Invalid, 7));
        CHECK(tokens[16] == Token("?", Token::Type::UnaryOperator, 7));
    }

    SECTION("Invalid number tokens") {
        Lexer lexer("examples/lexer/LexerInvalidNumber.ct");
        auto tokens = GetTokens(lexer);
        INFO(PrintTokens(tokens));
        REQUIRE(tokens.size() == 6);
        CHECK(tokens[0] == Token("0...", Token::Type::Invalid, 2));
        CHECK(tokens[1] == Token("1.2.3", Token::Type::Invalid, 3));
        CHECK(tokens[2] == Token("4.", Token::Type::Invalid, 4));
        CHECK(tokens[3] == Token(".", Token::Type::BinaryOperator, 5));
        CHECK(tokens[4] == Token("5", Token::Type::Number, 5));
    }

    SECTION("Parentheses") {
        Lexer lexer("examples/lexer/LexerParentheses.ct");
        auto tokens = GetTokens(lexer);
        INFO(PrintTokens(tokens));
        REQUIRE(tokens.size() == 12);
        CHECK(tokens[1] == Token("[", Token::Type::SquareOpen, 2));
        CHECK(tokens[2] == Token("}", Token::Type::CurlyClose, 2));
        CHECK(tokens[6] == Token("{", Token::Type::CurlyOpen, 2));
        CHECK(tokens[7] == Token(")", Token::Type::ParenClose, 3));
        CHECK(tokens[10] == Token("(", Token::Type::ParenOpen, 3));
    }

    SECTION("Keywords") {
        Lexer lexer("examples/lexer/LexerKeywords.ct");
        auto tokens = GetTokens(lexer);
        INFO(PrintTokens(tokens));
        REQUIRE(tokens.size() == 13);
        CHECK(tokens[0] == Token("for", Token::Type::For, 2));
        CHECK(tokens[1] == Token("+", Token::Type::BinaryOperator, 2));
        CHECK(tokens[2] == Token("foodo", Token::Type::Identifier, 2));
        CHECK(tokens[3] == Token("(", Token::Type::ParenOpen, 2));
        CHECK(tokens[4] == Token("iffor", Token::Type::Identifier, 2));
        CHECK(tokens[5] == Token(")", Token::Type::ParenClose, 2));
        CHECK(tokens[6] == Token("0", Token::Type::Number, 3));
        CHECK(tokens[7] == Token("+", Token::Type::BinaryOperator, 3));
        CHECK(tokens[8] == Token("if", Token::Type::If, 3));
        CHECK(tokens[9] == Token(".", Token::Type::BinaryOperator, 3));
        CHECK(tokens[10] == Token("0.1", Token::Type::Number, 3));
        CHECK(tokens[11] == Token("func", Token::Type::Func, 3));
    }

    SECTION("More keywords on complex example") {
        Lexer lexer("examples/lexer/LexerComplex.ct");
        auto tokens = GetTokens(lexer);
        INFO(PrintTokens(tokens));
        REQUIRE(tokens.size() == 42);
        CHECK(tokens[0] == Token("func", Token::Type::Func, 4));
        CHECK(tokens[1] == Token("goo", Token::Type::Identifier, 4));
        CHECK(tokens[2] == Token("(", Token::Type::ParenOpen, 4));
        CHECK(tokens[3] == Token("_", Token::Type::Identifier, 4));
        CHECK(tokens[4] == Token("bar", Token::Type::Identifier, 4));
        CHECK(tokens[5] == Token(":", Token::Type::Colon, 4));
        CHECK(tokens[6] == Token("string", Token::Type::Identifier, 4));
        CHECK(tokens[7] == Token(",", Token::Type::Comma, 4));
        CHECK(tokens[11] == Token(")", Token::Type::ParenClose, 4));
        CHECK(tokens[12] == Token("->", Token::Type::BinaryOperator, 4));
        CHECK(tokens[13] == Token("bool", Token::Type::Identifier, 4));
        CHECK(tokens[14] == Token("{", Token::Type::CurlyOpen, 4));
        CHECK(tokens[15] == Token("var", Token::Type::Var, 5));
        CHECK(tokens[16] == Token("f", Token::Type::Identifier, 5));
        CHECK(tokens[17] == Token("=", Token::Type::BinaryOperator, 5));
        CHECK(tokens[18] == Token("null", Token::Type::Null, 5));
        CHECK(tokens[19] == Token(";", Token::Type::Semicolon, 5));
        CHECK(tokens[20] == Token("return", Token::Type::Return, 5));
        CHECK(tokens[21] == Token("true", Token::Type::True, 5));
        CHECK(tokens[23] == Token("for", Token::Type::For, 7));
        CHECK(tokens[24] == Token("i", Token::Type::Identifier, 7));
        CHECK(tokens[25] == Token("in", Token::Type::In, 7));
        CHECK(tokens[26] == Token("0", Token::Type::Number, 7));
        CHECK(tokens[27] == Token("..<", Token::Type::BinaryOperator, 7));
        CHECK(tokens[28] == Token("string01", Token::Type::Identifier, 7));
        CHECK(tokens[30] == Token("size", Token::Type::Identifier, 7));
        CHECK(tokens[33] == Token("[", Token::Type::SquareOpen, 8));
        CHECK(tokens[38] == Token(".", Token::Type::BinaryOperator, 8));
    }
}

TEST_CASE("Parser") {
    SECTION("Simple parser test") {
        Parser p("examples/parser/ParserSimple.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 1:goo, }\n"
              "    FunctionDef: {\n"
              "        Name: goo\n"
              "        Symbols: { 3:boo, 2:foo, }\n"
              "        Arguments: {\n"
              "            Variable: foo\n"
              "            Variable: boo\n"
              "        }\n"
              "        Block: {\n"
              "            String: \"a\"\n"
              "            Double: 1\n"
              "        }\n"
              "    }\n"
              "}\n");
    }

    SECTION("For loop parsing") {
        Parser p("examples/parser/ParserForLoop.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 1:goo, }\n"
              "    FunctionDef: {\n"
              "        Name: goo\n"
              "        Symbols: { 2:bbb, 4:ccc, 3:yyy, }\n"
              "        Arguments: {\n"
              "            Variable: bbb\n"
              "            Variable: yyy\n"
              "            Variable: ccc\n"
              "        }\n"
              "        Block: {\n"
              "            For: {\n"
              "                Symbols: { 5:aaa, }\n"
              "                Variable: aaa\n"
              "                Range: {\n"
              "                    Variable: bbb\n"
              "                }\n"
              "                Block: {\n"
              "                    For: {\n"
              "                        Symbols: { 6:xxx, }\n"
              "                        Variable: xxx\n"
              "                        Range: {\n"
              "                            Variable: yyy\n"
              "                        }\n"
              "                        Block: {\n"
              "                            Variable: ccc\n"
              "                        }\n"
              "                    }\n"
              "                }\n"
              "            }\n"
              "        }\n"
              "    }\n"
              "}\n");
    }

    SECTION("Function call parsing") {
        Parser p("examples/parser/ParserFunctionCall.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 1:goo, }\n"
              "    FunctionDef: {\n"
              "        Name: goo\n"
              "        Symbols: { 2:a, }\n"
              "        Arguments: {\n"
              "            Variable: a\n"
              "        }\n"
              "        Block: {\n"
              "            FunctionCall: {\n"
              "                Name: goo\n"
              "                Arguments: {\n"
              "                    Variable: a\n"
              "                }\n"
              "            }\n"
              "        }\n"
              "    }\n"
              "}\n");
    }

    SECTION("Wrong parser files") {
        CHECK_THROWS_WITH(TryCreateParser("parser/WrongGlobal.ct"),
                          "Failed to parse [Identifier 'foo' on line 2]. Expected Func.");
        CHECK_THROWS_WITH(TryCreateParser("parser/WrongFuncArg.ct"), "Failed to parse [Identifier 'a' on line 2]. Expected Var.");
        CHECK_THROWS_WITH(TryCreateParser("parser/WrongFuncComma.ct"), "Failed to parse [Bracket ')' on line 2]. Expected Var.");
        CHECK_THROWS_WITH(TryCreateParser("parser/WrongFuncBlock.ct"),
                          "Failed to parse [Bracket '}' on line 3]. Expected Bracket.");
        CHECK_THROWS_WITH(TryCreateParser("parser/WrongStatement.ct"),
                          "Failed to parse [Func 'func' on line 3]. Expected one of following { For, If, While, Do, "
                          "Identifier, String, Number, Var, }.");
    }
}

TEST_CASE("Symbol Table") {
    SECTION("Function names") {
        Parser p("examples/symbols/FunctionDef.ct");
        CHECK(p.Tree().Root().Symbols().GetSymbol("foo") == 1);
        CHECK(p.Tree().Root().Symbols().GetSymbol("goo") == 2);
    }

    SECTION("Undefined symbol exception") {
        Parser p("examples/symbols/FunctionDef.ct");
        INFO(p.Tree());
        CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowUndefinedFunc.ct"), "Found undefined identifier a.");
        CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowForLoopIdentifierOutside.ct"), "Found undefined identifier a.");
        CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowSymbolFromAnotherBlock.ct"), "Found undefined identifier a.");
        CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowUseBeforeDeclaration.ct"), "Found undefined identifier a.");
    }
}