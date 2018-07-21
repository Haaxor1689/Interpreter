#include "catch.hpp"

#include <sstream>

#include "Interpreter"

using namespace std;
using namespace Interpreter;

namespace {
std::string to_string(const Value& value) {
    ostringstream oss;
    oss << value;
    return oss.str();
}
} // namespace

TEST_CASE("TicTacToe") {
    SECTION("Compiles correctly") {
        REQUIRE_NOTHROW(Parser("examples/TicTacToe.ct"));
    }

    SECTION("IsValidColumn") {
        Parser p("examples/TicTacToe.ct");
        INFO(p.Tree());
        CHECK(to_string(p.Evaluate("IsValidColumn", { "1" })) == "True");
        CHECK(to_string(p.Evaluate("IsValidColumn", { "-1" })) == "False");
        CHECK(to_string(p.Evaluate("IsValidColumn", { "3" })) == "False");
    }
}