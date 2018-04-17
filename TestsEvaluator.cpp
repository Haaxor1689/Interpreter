#include "catch.hpp"

#include <sstream>

#include "Interpreter"

using namespace std;
using namespace Interpreter;

TEST_CASE("Evaluator") {
    SECTION("Simple functions") {
        Parser p("examples/evaluator/Empty.ct");
        CHECK(std::holds_alternative<std::monostate>(p.Evaluate("foo", {})));

        Parser q("examples/evaluator/BoolReturn.ct");
        CHECK(std::get<bool>(q.Evaluate("foo")) == true);

        Parser r("examples/evaluator/DoubleReturn.ct");
        CHECK(std::get<double>(r.Evaluate("foo")) == 12.4);

        Parser s("examples/evaluator/StringReturn.ct");
        CHECK(std::get<std::string>(s.Evaluate("foo")) == "Test");

        Parser t("examples/evaluator/ArgumentReturn.ct");
        CHECK(std::get<bool>(t.Evaluate("foo", { true })) == true);
        CHECK(std::get<double>(t.Evaluate("foo", { 123.0 })) == 123.0);
        CHECK(std::get<std::string>(t.Evaluate("foo", { std::string("goo") })) == "goo");
    }

    SECTION("If Statement") {
        Parser p("examples/evaluator/IfExample.ct");
    }
}