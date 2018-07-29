#include "catch.hpp"

#include <sstream>

#include "../Interpreter"

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

} // namespace

namespace LexerTests {

TEST_CASE("Empty file gives EoF token") {
    std::ifstream file("examples/lexer/LexerEmpty.ct");
    Lexer lexer(file);
    auto tokens = GetTokens(lexer);
    INFO(PrintTokens(tokens));
    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0] == Token("", Token::Type::EoF, 1));
}

TEST_CASE("File with only a comment") {
    std::ifstream file("examples/lexer/LexerComment.ct");
    Lexer lexer(file);
    auto tokens = GetTokens(lexer);
    INFO(PrintTokens(tokens));
    REQUIRE(tokens.size() == 1);
    CHECK(tokens[0] == Token("", Token::Type::EoF, 2));
}

TEST_CASE("Two identifiers split by space") {
    std::ifstream file("examples/lexer/LexerTwoIdentifiers.ct");
    Lexer lexer(file);
    auto tokens = GetTokens(lexer);
    INFO(PrintTokens(tokens));
    REQUIRE(tokens.size() == 3);
    CHECK(tokens[0] == Token("foo", Token::Type::Identifier, 2));
    CHECK(tokens[0] == Token("foo", Token::Type::Identifier, 2));
    CHECK(tokens[2] == Token("", Token::Type::EoF, 3));
}

TEST_CASE("Number tokens") {
    std::ifstream file("examples/lexer/LexerNumbers.ct");
    Lexer lexer(file);
    auto tokens = GetTokens(lexer);
    INFO(PrintTokens(tokens));
    REQUIRE(tokens.size() == 4);
    CHECK(tokens[0] == Token("foo2", Token::Type::Identifier, 2));
    CHECK(tokens[1] == Token("2.1", Token::Type::Number, 2));
    CHECK(tokens[2] == Token("as", Token::Type::As, 2));
}

TEST_CASE("String tokens") {
    std::ifstream file("examples/lexer/LexerStrings.ct");
    Lexer lexer(file);
    auto tokens = GetTokens(lexer);
    INFO(PrintTokens(tokens));
    REQUIRE(tokens.size() == 6);
    CHECK(tokens[0] == Token("foo", Token::Type::Identifier, 2));
    CHECK(tokens[1] == Token("\"\"", Token::Type::String, 2));
    CHECK(tokens[2] == Token("\"foo 1\"", Token::Type::String, 2));
    CHECK(tokens[3] == Token("3", Token::Type::Number, 2));
    CHECK(tokens[4] == Token("goo2", Token::Type::Identifier, 2));
}

TEST_CASE("Invalid string tokens") {
    std::ifstream file("examples/lexer/LexerInvalidString.ct");
    Lexer lexer(file);
    auto tokens = GetTokens(lexer);
    INFO(PrintTokens(tokens));
    REQUIRE(tokens.size() == 5);
    CHECK(tokens[0] == Token("foo", Token::Type::Identifier, 2));
    CHECK(tokens[1] == Token("\"2\"", Token::Type::String, 2));
    CHECK(tokens[2] == Token("\"123", Token::Type::Invalid, 2));
    CHECK(tokens[3] == Token("goo", Token::Type::Identifier, 3));
    CHECK(tokens[4] == Token("", Token::Type::EoF, 4));
}

TEST_CASE("Operators") {
    std::ifstream file("examples/lexer/LexerOperators.ct");
    Lexer lexer(file);
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

TEST_CASE("Invalid number tokens") {
    std::ifstream file("examples/lexer/LexerInvalidNumber.ct");
    Lexer lexer(file);
    auto tokens = GetTokens(lexer);
    INFO(PrintTokens(tokens));
    REQUIRE(tokens.size() == 6);
    CHECK(tokens[0] == Token("0...", Token::Type::Invalid, 2));
    CHECK(tokens[1] == Token("1.2.3", Token::Type::Invalid, 3));
    CHECK(tokens[2] == Token("4.", Token::Type::Invalid, 4));
    CHECK(tokens[3] == Token(".", Token::Type::BinaryOperator, 5));
    CHECK(tokens[4] == Token("5", Token::Type::Number, 5));
}

TEST_CASE("Parentheses") {
    std::ifstream file("examples/lexer/LexerParentheses.ct");
    Lexer lexer(file);
    auto tokens = GetTokens(lexer);
    INFO(PrintTokens(tokens));
    REQUIRE(tokens.size() == 12);
    CHECK(tokens[1] == Token("[", Token::Type::SquareOpen, 2));
    CHECK(tokens[2] == Token("}", Token::Type::CurlyClose, 2));
    CHECK(tokens[6] == Token("{", Token::Type::CurlyOpen, 2));
    CHECK(tokens[7] == Token(")", Token::Type::ParenClose, 3));
    CHECK(tokens[10] == Token("(", Token::Type::ParenOpen, 3));
}

TEST_CASE("Keywords") {
    std::ifstream file("examples/lexer/LexerKeywords.ct");
    Lexer lexer(file);
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

TEST_CASE("More keywords on complex example") {
    std::ifstream file("examples/lexer/LexerComplex.ct");
    Lexer lexer(file);
    auto tokens = GetTokens(lexer);
    INFO(PrintTokens(tokens));
    REQUIRE(tokens.size() == 41);
    CHECK(tokens[0] == Token("func", Token::Type::Func, 4));
    CHECK(tokens[1] == Token("goo", Token::Type::Identifier, 4));
    CHECK(tokens[2] == Token("(", Token::Type::ParenOpen, 4));
    CHECK(tokens[3] == Token("bar", Token::Type::Identifier, 4));
    CHECK(tokens[4] == Token(":", Token::Type::Colon, 4));
    CHECK(tokens[5] == Token("string", Token::Type::Identifier, 4));
    CHECK(tokens[6] == Token(",", Token::Type::Comma, 4));
    CHECK(tokens[10] == Token(")", Token::Type::ParenClose, 4));
    CHECK(tokens[11] == Token(":", Token::Type::Colon, 4));
    CHECK(tokens[12] == Token("bool", Token::Type::Identifier, 4));
    CHECK(tokens[13] == Token("{", Token::Type::CurlyOpen, 4));
    CHECK(tokens[14] == Token("var", Token::Type::Var, 5));
    CHECK(tokens[15] == Token("f", Token::Type::Identifier, 5));
    CHECK(tokens[16] == Token("=", Token::Type::BinaryOperator, 5));
    CHECK(tokens[17] == Token("null", Token::Type::Null, 5));
    CHECK(tokens[18] == Token(";", Token::Type::Semicolon, 5));
    CHECK(tokens[19] == Token("return", Token::Type::Return, 5));
    CHECK(tokens[20] == Token("true", Token::Type::True, 5));
    CHECK(tokens[22] == Token("for", Token::Type::For, 7));
    CHECK(tokens[23] == Token("i", Token::Type::Identifier, 7));
    CHECK(tokens[24] == Token("in", Token::Type::In, 7));
    CHECK(tokens[25] == Token("0", Token::Type::Number, 7));
    CHECK(tokens[26] == Token("..<", Token::Type::RangeOperator, 7));
    CHECK(tokens[27] == Token("string01", Token::Type::Identifier, 7));
    CHECK(tokens[29] == Token("size", Token::Type::Identifier, 7));
    CHECK(tokens[32] == Token("[", Token::Type::SquareOpen, 8));
    CHECK(tokens[37] == Token(".", Token::Type::BinaryOperator, 8));
}

TEST_CASE("Negative numbers") {
    std::ifstream file("examples/lexer/LexerNegativeNumbers.ct");
    Lexer lexer(file);
    auto tokens = GetTokens(lexer);
    INFO(PrintTokens(tokens));
    REQUIRE(tokens.size() == 5);
    CHECK(tokens[0] == Token("-1", Token::Type::Number, 2));
    CHECK(tokens[1] == Token("-.2", Token::Type::Number, 2));
    CHECK(tokens[2] == Token("-", Token::Type::BinaryOperator, 2));
    CHECK(tokens[3] == Token("2", Token::Type::Number, 2));
}

TEST_CASE("From string") {
    std::istringstream source("func foo() { 1 }");
    Lexer lexer(source);
    auto tokens = GetTokens(lexer);
    INFO(PrintTokens(tokens));
    REQUIRE(tokens.size() == 8);
    CHECK(tokens[0] == Token("func", Token::Type::Func, 1));
    CHECK(tokens[1] == Token("foo", Token::Type::Identifier, 1));
    CHECK(tokens[2] == Token("(", Token::Type::ParenOpen, 1));
    CHECK(tokens[3] == Token(")", Token::Type::ParenClose, 1));
    CHECK(tokens[4] == Token("{", Token::Type::CurlyOpen, 1));
    CHECK(tokens[5] == Token("1", Token::Type::Number, 1));
    CHECK(tokens[6] == Token("}", Token::Type::CurlyClose, 1));
}

} // namespace LexerTests
