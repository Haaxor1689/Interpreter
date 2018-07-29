#include "catch.hpp"

#include <sstream>

#include "../Interpreter"

using namespace std;
using namespace Interpreter;

namespace TicTacToeTests {

TEST_CASE("Compiles correctly") {
    REQUIRE_NOTHROW(Parser("examples/TicTacToe.ct"));
}

TEST_CASE("IsValidColumn") {
    Parser p("examples/TicTacToe.ct");
    CHECK(ToString(p.Evaluate("IsValidColumn", { 1.0 })) == "True");
    CHECK(ToString(p.Evaluate("IsValidColumn", { -1.0 })) == "False");
    CHECK(ToString(p.Evaluate("IsValidColumn", { 3.0 })) == "False");
}

TEST_CASE("IsValidRow") {
    Parser p("examples/TicTacToe.ct");
    CHECK(ToString(p.Evaluate("IsValidRow", { "A"s })) == "True");
    CHECK(ToString(p.Evaluate("IsValidRow", { "C"s })) == "True");
    CHECK(ToString(p.Evaluate("IsValidRow", { "D"s })) == "False");
    CHECK(ToString(p.Evaluate("IsValidRow", { "b"s })) == "False");
}

} // namespace TicTacToeTests
