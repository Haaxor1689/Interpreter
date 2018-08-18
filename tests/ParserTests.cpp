#include "catch.hpp"

#include <sstream>

#include "../Interpreter"

using namespace std;
using namespace Interpreter;

namespace {

void CompareTree(const std::string& fileName) {
    Parser p("examples/parser/" + fileName + ".ct");
    ostringstream actual;
    actual << p.Tree();

    ifstream source("examples/parser/" + fileName + "Tree.json");
    ostringstream expected;
    expected << source.rdbuf();

    CHECK(actual.str() == expected.str());
}

void TryCreateParser(const std::string& name) {
    Parser p("examples/" + name);
}

} // namespace

namespace ParserTests {

TEST_CASE("Simple parser test") {
    CompareTree("Simple");
}

TEST_CASE("For loop parsing") {
    CompareTree("ForLoop");
}

TEST_CASE("Function call parsing") {
    CompareTree("FunctionCall");
}

TEST_CASE("If/Else parsing") {
    CompareTree("IfElse");
}

TEST_CASE("If without else") {
    CompareTree("If");
}

TEST_CASE("While parsing") {
    CompareTree("While");
}

TEST_CASE("DoWhile parsing") {
    CompareTree("DoWhile");
}

TEST_CASE("Assignment parsing") {
    CompareTree("Assignment");
}

TEST_CASE("Operators parser") {
    CompareTree("Operators");
}

TEST_CASE("Function and variables types") {
    CompareTree("WithTypes");
}

TEST_CASE("Range parser") {
    CompareTree("Range");
}

TEST_CASE("Object definition") {
    CompareTree("ObjectDef");
}

TEST_CASE("Object init parser") {
    CompareTree("ObjectInit");
}

TEST_CASE("Index operation parsing") {
    CompareTree("IndexOperation");
}

TEST_CASE("Wrong parser files") {
    CHECK_THROWS_WITH(TryCreateParser("parser/WrongGlobal.ct"),
        "Failed to parse [Identifier 'foo' on line 2]. Expected one of following { Func, Object, }.");
    CHECK_THROWS_WITH(TryCreateParser("parser/WrongFuncArg.ct"), "Failed to parse [Identifier 'a' on line 2]. Expected Var.");
    CHECK_THROWS_WITH(TryCreateParser("parser/WrongFuncComma.ct"), "Failed to parse [Bracket ')' on line 2]. Expected Var.");
    CHECK_THROWS_WITH(TryCreateParser("parser/WrongFuncBlock.ct"),
        "Failed to parse [Bracket '}' on line 3]. Expected Bracket.");
    CHECK_THROWS_WITH(TryCreateParser("parser/WrongStatement.ct"),
        "Failed to parse [Func 'func' on line 3]. Expected one of following { Return, For, If, While, Do, Unary Operator, Binary Operator, Identifier, Var, New, Bracket, True, False, Number, String, }."
    );
}

} // namespace ParserTests
