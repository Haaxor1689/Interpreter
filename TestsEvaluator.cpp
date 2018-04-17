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

TEST_CASE("Evaluator") {
    SECTION("Simple functions") {
        Parser p("examples/evaluator/Empty.ct");
        CHECK(to_string(p.Evaluate("foo")) == "Void");

        Parser q("examples/evaluator/BoolReturn.ct");
        CHECK(to_string(q.Evaluate("foo")) == "True");

        Parser r("examples/evaluator/DoubleReturn.ct");
        CHECK(to_string(r.Evaluate("foo")) == "12.4");

        Parser s("examples/evaluator/StringReturn.ct");
        CHECK(to_string(s.Evaluate("foo")) == "Test");

        Parser t("examples/evaluator/ArgumentReturn.ct");
        CHECK(to_string(t.Evaluate("foo", { true })) == "True");
        CHECK(to_string(t.Evaluate("foo", { 123.0 })) == "123");
        CHECK(to_string(t.Evaluate("foo", { std::string("goo") })) == "goo");
    }

    SECTION("If Statement") {
        Parser p("examples/evaluator/IfExample.ct");
        CHECK(to_string(p.Evaluate("foo", { true })) == "1");
        CHECK(to_string(p.Evaluate("foo", { false })) == "0");
    }

    SECTION("Assignments") {
        Parser p("examples/evaluator/SimpleAssignment.ct");
        CHECK(to_string(p.Evaluate("foo", { true })) == "True");
        CHECK(to_string(p.Evaluate("foo", { false })) == "False");

        Parser q("examples/evaluator/ConditionalAssignment.ct");
        CHECK(to_string(q.Evaluate("foo", { true })) == "1");
        CHECK(to_string(q.Evaluate("foo", { false })) == "Void");
    }
}