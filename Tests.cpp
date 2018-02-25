#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Parser.hpp"

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

    void PrintTokens(const vector<Token>& tokens) {
        for (const auto& token : tokens)
            cout << token << endl;
    }
}

TEST_CASE("Lexer") {
    SECTION("Empty file gives EoF token") {
        cout << "\nEmpty file gives EoF token\n";
        Lexer lexer("examples/LexerEmpty.ct");
        auto tokens = GetTokens(lexer);
        PrintTokens(tokens);
        REQUIRE(tokens.size() == 1);
        CHECK(tokens[0] == Token("", Token::Type::EoF, 1));
    }

    SECTION("File with only a comment") {
        cout << "\nFile with only a comment\n";
        Lexer lexer("examples/LexerComment.ct");
        auto tokens = GetTokens(lexer);
        PrintTokens(tokens);
        REQUIRE(tokens.size() == 1);
        CHECK(tokens[0] == Token("", Token::Type::EoF, 2));
    }

    SECTION("Two identifiers split by space") {
        cout << "\nTwo identifiers split by space\n";
        Lexer lexer("examples/LexerTwoIdentifiers.ct");
        auto tokens = GetTokens(lexer);
        PrintTokens(tokens);
        REQUIRE(tokens.size() == 3);
        CHECK(tokens[0] == Token("foo", Token::Type::Identifier, 2));
        CHECK(tokens[0] == Token("foo", Token::Type::Identifier, 2));
        CHECK(tokens[2] == Token("", Token::Type::EoF, 3));
    }

    SECTION("Number tokens") {
        cout << "\nNumber tokens\n";
        Lexer lexer("examples/LexerNumbers.ct");
        auto tokens = GetTokens(lexer);
        PrintTokens(tokens);
        REQUIRE(tokens.size() == 4);
        CHECK(tokens[0] == Token("foo2", Token::Type::Identifier, 2));
        CHECK(tokens[1] == Token("2.1", Token::Type::Number, 2));
        CHECK(tokens[2] == Token("as", Token::Type::Identifier, 2));
    }

    SECTION("String tokens") {
        cout << "\nString tokens\n";
        Lexer lexer("examples/LexerStrings.ct");
        auto tokens = GetTokens(lexer);
        PrintTokens(tokens);
        REQUIRE(tokens.size() == 6);
        CHECK(tokens[0] == Token("foo", Token::Type::Identifier, 2));
        CHECK(tokens[1] == Token("\"\"", Token::Type::String, 2)); 
        CHECK(tokens[2] == Token("\"foo 1\"", Token::Type::String, 2));
        CHECK(tokens[3] == Token("3", Token::Type::Number, 2));
        CHECK(tokens[4] == Token("goo2", Token::Type::Identifier, 2));
    }

    SECTION("Invalid string tokens") {
        cout << "\nInvalid string tokens\n";
        Lexer lexer("examples/LexerInvalidString.ct");
        auto tokens = GetTokens(lexer);
        PrintTokens(tokens);
        REQUIRE(tokens.size() == 5);
        CHECK(tokens[0] == Token("foo", Token::Type::Identifier, 2));
        CHECK(tokens[1] == Token("\"2\"", Token::Type::String, 2));
        CHECK(tokens[2] == Token("\"123", Token::Type::Invalid, 2));
        CHECK(tokens[3] == Token("goo", Token::Type::Identifier, 3));
        CHECK(tokens[4] == Token("", Token::Type::EoF, 4));
    }

    SECTION("Operators") {
        cout << "\nOperators\n";
        Lexer lexer("examples/LexerOperators.ct");
        auto tokens = GetTokens(lexer);
        PrintTokens(tokens);
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
        cout << "\nInvalid number tokens\n";
        Lexer lexer("examples/LexerInvalidNumber.ct");
        auto tokens = GetTokens(lexer);
        PrintTokens(tokens);
        REQUIRE(tokens.size() == 6);
        CHECK(tokens[0] == Token("0...", Token::Type::Invalid, 2));
        CHECK(tokens[1] == Token("1.2.3", Token::Type::Invalid, 3));
        CHECK(tokens[2] == Token("4.", Token::Type::Invalid, 4));
        CHECK(tokens[3] == Token(".", Token::Type::BinaryOperator, 5));
        CHECK(tokens[4] == Token("5", Token::Type::Number, 5));
    }

    SECTION("Parentheses") {
        cout << "\nParentheses\n";
        Lexer lexer("examples/LexerParentheses.ct");
        auto tokens = GetTokens(lexer);
        PrintTokens(tokens);
        REQUIRE(tokens.size() == 12);
        CHECK(tokens[1] == Token("[", Token::Type::SquareOpen, 2));
        CHECK(tokens[2] == Token("}", Token::Type::CurlyClose, 2));
        CHECK(tokens[6] == Token("{", Token::Type::CurlyOpen, 2));
        CHECK(tokens[7] == Token(")", Token::Type::ParenClose, 3));
        CHECK(tokens[10] == Token("(", Token::Type::ParenOpen, 3));
    }
}

TEST_CASE("Parser") {

}

