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
    CHECK(ToString(p.Evaluate("IsValidColumn", { "A"s })) == "True");
    CHECK(ToString(p.Evaluate("IsValidColumn", { "C"s })) == "True");
    CHECK(ToString(p.Evaluate("IsValidColumn", { "c"s })) == "False");
}

TEST_CASE("IsValidRow") {
    Parser p("examples/TicTacToe.ct");
    CHECK(ToString(p.Evaluate("IsValidRow", { "Y"s })) == "True");
    CHECK(ToString(p.Evaluate("IsValidRow", { "X"s })) == "True");
    CHECK(ToString(p.Evaluate("IsValidRow", { "x"s })) == "False");
    CHECK(ToString(p.Evaluate("IsValidRow", { "XY"s })) == "False");
}

} // namespace TicTacToeTests
