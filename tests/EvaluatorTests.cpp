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
    CHECK_THROWS_WITH(ToString(p.Evaluate("foo", { "Hello"s, 1.0 })), "An exception occured on line 2. Message: Invalid operator use.");
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
    CHECK_THROWS_WITH(p.Evaluate("foo", { "a" }), "An exception occured on line 2. Message: Invalid operator use.");
}

TEST_CASE("Recursive call") {
    Parser p("examples/evaluator/FunctionRecursiveCall.ct");
    INFO(p.Tree());
    CHECK(ToString(p.Evaluate("Factorial", { 1.0 })) == "1");
    CHECK(ToString(p.Evaluate("Factorial", { 2.0 })) == "2");
    CHECK(ToString(p.Evaluate("Factorial", { 4.0 })) == "24");
}

TEST_CASE("Negation operator") {
    Parser p("examples/evaluator/NegationOperator.ct");
    INFO(p.Tree());

    CHECK_NOTHROW(p.Evaluate("foo", { bool() }));
    CHECK_NOTHROW(std::get<bool>(p.Evaluate("foo", { bool() })));

    CHECK(ToString(p.Evaluate("foo", { true })) == "False");
    CHECK(ToString(p.Evaluate("foo", { false })) == "True");
    
    CHECK_THROWS_WITH(p.Evaluate("foo", { "abc"s }), "An exception occured on line 2. Message: Invalid operator use.");
    CHECK_THROWS_WITH(p.Evaluate("foo", { 1.0 }), "An exception occured on line 2. Message: Invalid operator use.");
}

TEST_CASE("Increment operator") {
    Parser p("examples/evaluator/IncrementOperator.ct");
    INFO(p.Tree());

    CHECK_NOTHROW(p.Evaluate("foo", { double() }));
    CHECK_NOTHROW(std::get<double>(p.Evaluate("foo", { double() })));

    CHECK(ToString(p.Evaluate("foo", { 1.0 })) == "2");
    CHECK(ToString(p.Evaluate("foo", { -1.0 })) == "0");
    
    CHECK_THROWS_WITH(p.Evaluate("foo", { "abc"s }), "An exception occured on line 2. Message: Invalid operator use.");
    CHECK_THROWS_WITH(p.Evaluate("foo", { true }), "An exception occured on line 2. Message: Invalid operator use.");
}

TEST_CASE("While loop") {
    Parser p("examples/evaluator/WhileLoop.ct");
    INFO(p.Tree());

    CHECK(ToString(p.Evaluate("foo", { 1.0, 5.0 })) == "5");
    CHECK(ToString(p.Evaluate("foo", { -1.0, 12.0 })) == "0");
    CHECK(ToString(p.Evaluate("foo", { 5.0, 5.0 })) == "25");
}

TEST_CASE("For loop") {
    Parser p("examples/evaluator/ForLoop.ct");
    INFO(p.Tree());

    CHECK(ToString(p.Evaluate("foo", { 5.0 })) == "14");
    CHECK(ToString(p.Evaluate("foo", { -3.0 })) == "-3");
    CHECK(ToString(p.Evaluate("foo", { 2.0 })) == "2");
}

TEST_CASE("Object") {
    Parser p("examples/evaluator/Object.ct");
    INFO(p.Tree());

    CHECK(ToString(p.Evaluate("foo", { 5.0 })) == "{ a: 5, b: \"hello\", }");
}

TEST_CASE("Object attribute assign") {
    Parser p("examples/evaluator/ObjectAssign.ct");
    INFO(p.Tree());

    CHECK(ToString(p.Evaluate("foo", { 10.0 })) == "{ a: 10, }");
}

TEST_CASE("Dot operator chaining evaluation") {
    Parser p("examples/evaluator/DotOperatorChaining.ct");
    INFO(p.Tree());

    CHECK(ToString(p.Evaluate("foo", { "Hello"s })) == "{ a: { value: \"Hello\", }, b: { value: \"HelloWorld\", }, }");
}

} // namespace EvaluatorTests
