#include "catch.hpp"

#include "Interpreter"

using namespace std;
using namespace Interpreter;

TEST_CASE("Function call") {
    SECTION("Missing return") {
        Parser p("examples/typecheck/MissingReturn.ct");
    }
}