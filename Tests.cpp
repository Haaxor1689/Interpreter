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
            if (tokens.back().type == Token::Type::EoF) {
                return tokens;
            }
        }
    }

    void PrintTokens(const vector<Token>& tokens) {
        for (const auto& token : tokens) {
            cout << token << endl;
        }
    }
}

TEST_CASE("Lexer") {
    SECTION("Empty file gives EoF token") {
        cout << "Empty file gives EoF token\n";
        Lexer lexer("examples/Lexer00.ct");
        auto tokens = GetTokens(lexer);
        PrintTokens(tokens);
        REQUIRE(tokens.size() == 1);

        CHECK(tokens[0].type == Token::Type::EoF);
        CHECK(tokens[0].text == "");
        CHECK(tokens[0].line == 1);
    }

    SECTION("File with only a comment") {
        cout << "File with only a comment\n";
        Interpreter::Lexer lexer("examples/Lexer01.ct");
        auto tokens = GetTokens(lexer);
        PrintTokens(tokens);
        REQUIRE(tokens.size() == 1);

        CHECK(tokens[0].type == Token::Type::EoF);
        CHECK(tokens[0].text == "");
        CHECK(tokens[0].line == 2);
    }

    SECTION("Two tokens split by space") {
        cout << "Two tokens split by space\n";
        Interpreter::Lexer lexer("examples/Lexer02.ct");
        auto tokens = GetTokens(lexer);
        PrintTokens(tokens);
        REQUIRE(tokens.size() == 3);

        CHECK(tokens[0].type == Token::Type::Identifier);
        CHECK(tokens[0].text == "foo");
        CHECK(tokens[0].line == 2);

        CHECK(tokens[1].type == Token::Type::Identifier);
        CHECK(tokens[1].text == "foo");
        CHECK(tokens[1].line == 2);

        CHECK(tokens[2].type == Token::Type::EoF);
        CHECK(tokens[2].text == "");
        CHECK(tokens[2].line == 3);
    }
}

TEST_CASE("Parser") {

}

