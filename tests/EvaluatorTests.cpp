#include "catch.hpp"

#include "../Interpreter"

using namespace std;
using namespace Interpreter;

namespace EvaluatorTests {

TEST_CASE("Simple functions") {
    Parser p("examples/evaluator/Empty.ct");
    INFO(p.Tree());
    CHECK(ToString(p.Evaluate("foo")) == "Void");

    Parser q("examples/evaluator/BoolReturn.ct");
    INFO(q.Tree());
    CHECK(ToString(q.Evaluate("foo")) == "True");

    Parser r("examples/evaluator/DoubleReturn.ct");
    INFO(r.Tree());
    CHECK(ToString(r.Evaluate("foo")) == "12.4");

    Parser s("examples/evaluator/StringReturn.ct");
    INFO(s.Tree());
    CHECK(ToString(s.Evaluate("foo")) == "Test");

    Parser t("examples/evaluator/ArgumentReturn.ct");
    INFO(t.Tree());
    CHECK(ToString(t.Evaluate("foo", { true })) == "True");
    CHECK(ToString(t.Evaluate("foo", { 123.0 })) == "123");
    CHECK(ToString(t.Evaluate("foo", { "goo"s })) == "goo");
}

TEST_CASE("If Statement") {
    Parser p("examples/evaluator/IfExample.ct");
    INFO(p.Tree());
    CHECK(ToString(p.Evaluate("foo", { true })) == "1");
    CHECK(ToString(p.Evaluate("foo", { false })) == "0");
}

TEST_CASE("Assignments") {
    Parser p("examples/evaluator/SimpleAssignment.ct");
    INFO(p.Tree());
    CHECK(ToString(p.Evaluate("foo", { true })) == "True");
    CHECK(ToString(p.Evaluate("foo", { false })) == "False");

    Parser q("examples/evaluator/ConditionalAssignment.ct");
    INFO(q.Tree());
    CHECK(ToString(q.Evaluate("foo", { true })) == "1");
    CHECK(ToString(q.Evaluate("foo", { false })) == "Void");
}

TEST_CASE("Equality Operator") {
    Parser p("examples/evaluator/EqualityOperator.ct");
    INFO(p.Tree());
    CHECK(ToString(p.Evaluate("foo", { 2.0, 2.0 })) == "0");
    CHECK(ToString(p.Evaluate("foo", { 3.0, 2.0 })) == "1");
    CHECK(ToString(p.Evaluate("foo", { 1.0, 2.0 })) == "-1");
}

TEST_CASE("Plus Operator") {
    Parser p("examples/evaluator/PlusOperator.ct");
    INFO(p.Tree());
    CHECK(ToString(p.Evaluate("foo", { 1.0, 1.0 })) == "2");
    CHECK(ToString(p.Evaluate("foo", { 2.5, -1.0 })) == "1.5");
    CHECK(ToString(p.Evaluate("foo", { "Hello"s, "World"s })) == "HelloWorld");
    CHECK_THROWS_WITH(ToString(p.Evaluate("foo", { "Hello"s, 1.0 })), "No operator for this type.");
}

TEST_CASE("Logical And Operator") {
    Parser p("examples/evaluator/LogicalAnd.ct");
    INFO(p.Tree());
    CHECK(ToString(p.Evaluate("foo", { true, true })) == "True");
    CHECK(ToString(p.Evaluate("foo", { true, false })) == "False");
    CHECK(ToString(p.Evaluate("foo", { false, true })) == "False");
    CHECK(ToString(p.Evaluate("foo", { false, false })) == "False");
}

TEST_CASE("Logical Or Operator") {
    Parser p("examples/evaluator/LogicalOr.ct");
    INFO(p.Tree());
    CHECK(ToString(p.Evaluate("foo", { true, true })) == "True");
    CHECK(ToString(p.Evaluate("foo", { true, false })) == "True");
    CHECK(ToString(p.Evaluate("foo", { false, true })) == "True");
    CHECK(ToString(p.Evaluate("foo", { false, false })) == "False");
}

TEST_CASE("Function call") {
    Parser p("examples/evaluator/FunctionCall.ct");
    INFO(p.Tree());
    CHECK(ToString(p.Evaluate("foo", { 1.0 })) == "2");
    CHECK_THROWS_WITH(p.Evaluate("foo", { "a" }), "No operator for this type.");
}

TEST_CASE("Recursive call") {
    Parser p("examples/evaluator/FunctionRecursiveCall.ct");
    INFO(p.Tree());
    CHECK(ToString(p.Evaluate("Factorial", { 1.0 })) == "1");
    CHECK(ToString(p.Evaluate("Factorial", { 2.0 })) == "2");
    CHECK(ToString(p.Evaluate("Factorial", { 4.0 })) == "24");
}

} // namespace EvaluatorTests
