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
        INFO(p.Tree());
        CHECK(to_string(p.Evaluate("foo")) == "Void");

        Parser q("examples/evaluator/BoolReturn.ct");
        INFO(q.Tree());
        CHECK(to_string(q.Evaluate("foo")) == "True");

        Parser r("examples/evaluator/DoubleReturn.ct");
        INFO(r.Tree());
        CHECK(to_string(r.Evaluate("foo")) == "12.4");

        Parser s("examples/evaluator/StringReturn.ct");
        INFO(s.Tree());
        CHECK(to_string(s.Evaluate("foo")) == "Test");

        Parser t("examples/evaluator/ArgumentReturn.ct");
        INFO(t.Tree());
        CHECK(to_string(t.Evaluate("foo", { true })) == "True");
        CHECK(to_string(t.Evaluate("foo", { 123.0 })) == "123");
        CHECK(to_string(t.Evaluate("foo", { "goo"s })) == "goo");
    }

    SECTION("If Statement") {
        Parser p("examples/evaluator/IfExample.ct");
        INFO(p.Tree());
        CHECK(to_string(p.Evaluate("foo", { true })) == "1");
        CHECK(to_string(p.Evaluate("foo", { false })) == "0");
    }

    SECTION("Assignments") {
        Parser p("examples/evaluator/SimpleAssignment.ct");
        INFO(p.Tree());
        CHECK(to_string(p.Evaluate("foo", { true })) == "True");
        CHECK(to_string(p.Evaluate("foo", { false })) == "False");

        Parser q("examples/evaluator/ConditionalAssignment.ct");
        INFO(q.Tree());
        CHECK(to_string(q.Evaluate("foo", { true })) == "1");
        CHECK(to_string(q.Evaluate("foo", { false })) == "Void");
    }

    SECTION("Equality Operator") {
        Parser p("examples/evaluator/EqualityOperator.ct");
        INFO(p.Tree());
        CHECK(to_string(p.Evaluate("foo", { 2.0, 2.0 })) == "0");
        CHECK(to_string(p.Evaluate("foo", { 3.0, 2.0 })) == "1");
        CHECK(to_string(p.Evaluate("foo", { 1.0, 2.0 })) == "-1");
    }

    SECTION("Plus Operator") {
        Parser p("examples/evaluator/PlusOperator.ct");
        INFO(p.Tree());
        CHECK(to_string(p.Evaluate("foo", { 1.0, 1.0 })) == "2");
        CHECK(to_string(p.Evaluate("foo", { 2.5, -1.0 })) == "1.5");
        CHECK(to_string(p.Evaluate("foo", { "Hello"s, "World"s })) == "HelloWorld");
        CHECK_THROWS_WITH(to_string(p.Evaluate("foo", { "Hello"s, 1.0 })), "No operator for this type.");
    }

    SECTION("Logical And Operator") {
        Parser p("examples/evaluator/LogicalAnd.ct");
        INFO(p.Tree());
        CHECK(to_string(p.Evaluate("foo", { true, true })) == "True");
        CHECK(to_string(p.Evaluate("foo", { true, false })) == "False");
        CHECK(to_string(p.Evaluate("foo", { false, true })) == "False");
        CHECK(to_string(p.Evaluate("foo", { false, false })) == "False");
    }

    SECTION("Logical Or Operator") {
        Parser p("examples/evaluator/LogicalOr.ct");
        INFO(p.Tree());
        CHECK(to_string(p.Evaluate("foo", { true, true })) == "True");
        CHECK(to_string(p.Evaluate("foo", { true, false })) == "True");
        CHECK(to_string(p.Evaluate("foo", { false, true })) == "True");
        CHECK(to_string(p.Evaluate("foo", { false, false })) == "False");
    }

    SECTION("Function call") {
        Parser p("examples/evaluator/FunctionCall.ct");
        INFO(p.Tree());
        CHECK(to_string(p.Evaluate("foo", { 1.0 })) == "2");
        CHECK_THROWS_WITH(p.Evaluate("foo", { "a" }), "No operator for this type.");
    }

    SECTION("Recursive call") {
        Parser p("examples/evaluator/FunctionRecursiveCall.ct");
        INFO(p.Tree());
        CHECK(to_string(p.Evaluate("Factorial", { 1.0 })) == "1");
        CHECK(to_string(p.Evaluate("Factorial", { 2.0 })) == "2");
        CHECK(to_string(p.Evaluate("Factorial", { 4.0 })) == "24");
    }
}