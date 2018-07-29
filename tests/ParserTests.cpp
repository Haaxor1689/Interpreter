#include "catch.hpp"

#include <sstream>

#include "../Interpreter"

using namespace std;
using namespace Interpreter;

namespace {

void TryCreateParser(const std::string& name) {
    Parser p("examples/" + name);
}

std::string prefix(const std::string& additionalSymbols = "") {
    return
            "Global: {\n"
            "    Symbols: { 10:ReadNumber, 11:ReadText, 6:Write, 8:WriteLine, 1:any, 3:bool, 12:foo, 5:number, 4:string, 2:void, }\n"
            "    FunctionDef: {\n"
            "        Name: foo\n"
            "        Symbols: { 13:a," + additionalSymbols + " }\n"
            "        Arguments: {\n"
            "            Definition: {\n"
            "                Variable: a\n"
            "                Type: any\n"
            "            }\n"
            "        }\n"
            "        Returns: {\n"
            "            Type: void\n"
            "        }\n"
            "        Block: {\n";
}

std::string postfix =
        "        }\n"
        "    }\n"
        "}\n";

} // namespace

namespace ParserTests {

TEST_CASE("Simple parser test") {
    Parser p("examples/parser/ParserSimple.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() == prefix() +
        "            String: \"a\"\n"
        "            Number: 1\n"
        + postfix);
}

TEST_CASE("For loop parsing") {
    Parser p("examples/parser/ParserForLoop.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() == prefix(" 14:bbb, 16:ccc, 15:yyy,") +
        "            Definition: {\n"
        "                Variable: bbb\n"
        "                Type: any\n"
        "            }\n"
        "            Definition: {\n"
        "                Variable: yyy\n"
        "                Type: any\n"
        "            }\n"
        "            Definition: {\n"
        "                Variable: ccc\n"
        "                Type: any\n"
        "            }\n"
        "            For: {\n"
        "                Symbols: { 17:aaa, }\n"
        "                ControlVariable: {\n"
        "                    Definition: {\n"
        "                        Variable: aaa\n"
        "                        Type: any\n"
        "                    }\n"
        "                }\n"
        "                In: {\n"
        "                    Variable: bbb\n"
        "                }\n"
        "                Block: {\n"
        "                    For: {\n"
        "                        Symbols: { 18:xxx, }\n"
        "                        ControlVariable: {\n"
        "                            Definition: {\n"
        "                                Variable: xxx\n"
        "                                Type: any\n"
        "                            }\n"
        "                        }\n"
        "                        In: {\n"
        "                            Variable: yyy\n"
        "                        }\n"
        "                        Block: {\n"
        "                            Variable: ccc\n"
        "                        }\n"
        "                    }\n"
        "                }\n"
        "            }\n"
        + postfix);
}

TEST_CASE("Function call parsing") {
    Parser p("examples/parser/ParserFunctionCall.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() == prefix() +
        "            FunctionCall: {\n"
        "                Name: foo\n"
        "                Arguments: {\n"
        "                    Variable: a\n"
        "                }\n"
        "            }\n"
        + postfix);
}

TEST_CASE("If/Else parsing") {
    Parser p("examples/parser/ParserIfElse.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() == prefix() +
        "            If: {\n"
        "                Symbols: { }\n"
        "                Condition: {\n"
        "                    Variable: a\n"
        "                }\n"
        "                Block: {\n"
        "                    Variable: a\n"
        "                }\n"
        "            }\n"
        "            Elseif: {\n"
        "                Symbols: { 14:b, }\n"
        "                Condition: {\n"
        "                    Definition: {\n"
        "                        Variable: b\n"
        "                        Type: any\n"
        "                    }\n"
        "                }\n"
        "                Block: {\n"
        "                    Variable: b\n"
        "                }\n"
        "            }\n"
        "            Else: {\n"
        "                Symbols: { }\n"
        "                Block: {\n"
        "                    Variable: a\n"
        "                }\n"
        "            }\n"
        "            Variable: a\n"
        + postfix);
}

TEST_CASE("If without else") {
    Parser p("examples/parser/ParserIf.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() == prefix() +
        "            If: {\n"
        "                Symbols: { }\n"
        "                Condition: {\n"
        "                    Variable: a\n"
        "                }\n"
        "                Block: {\n"
        "                }\n"
        "            }\n"
        + postfix);
}

TEST_CASE("While parsing") {
    Parser p("examples/parser/ParserWhile.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() == prefix() +
        "            While: {\n"
        "                Symbols: { }\n"
        "                Condition: {\n"
        "                    Variable: a\n"
        "                }\n"
        "                Block: {\n"
        "                }\n"
        "            }\n"
        + postfix);
}

TEST_CASE("DoWhile parsing") {
    Parser p("examples/parser/ParserDoWhile.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() == prefix() +
        "            DoWhile: {\n"
        "                Symbols: { }\n"
        "                Condition: {\n"
        "                    Variable: a\n"
        "                }\n"
        "                Block: {\n"
        "                }\n"
        "            }\n"
        + postfix);
}

TEST_CASE("Assignment parsing") {
    Parser p("examples/parser/ParserAssignment.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() ==
        "Global: {\n"
        "    Symbols: { 10:ReadNumber, 11:ReadText, 6:Write, 8:WriteLine, 1:any, 3:bool, 13:foo, 12:goo, 5:number, 4:string, 2:void, }\n"
        "    FunctionDef: {\n"
        "        Name: goo\n"
        "        Symbols: { }\n"
        "        Arguments: {\n"
        "        }\n"
        "        Returns: {\n"
        "            Type: void\n"
        "        }\n"
        "        Block: {\n"
        "        }\n"
        "    }\n"
        "    FunctionDef: {\n"
        "        Name: foo\n"
        "        Symbols: { 14:a, 15:b, 16:c, }\n"
        "        Arguments: {\n"
        "            Definition: {\n"
        "                Variable: a\n"
        "                Type: any\n"
        "            }\n"
        "        }\n"
        "        Returns: {\n"
        "            Type: void\n"
        "        }\n"
        "        Block: {\n"
        "            Definition: {\n"
        "                Variable: b\n"
        "                Type: any\n"
        "                Value: {\n"
        "                    Variable: a\n"
        "                }\n"
        "            }\n"
        "            Definition: {\n"
        "                Variable: c\n"
        "                Type: void\n"
        "            }\n"
        "            Assignment: {\n"
        "                Variable: c\n"
        "                Value: {\n"
        "                    FunctionCall: {\n"
        "                        Name: goo\n"
        "                        Arguments: {\n"
        "                        }\n"
        "                    }\n"
        "                }\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}\n");
}

TEST_CASE("Operators parser") {
    Parser p("examples/parser/ParserOperators.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() == prefix(" 14:b,") +
        "            Definition: {\n"
        "                Variable: b\n"
        "                Type: number\n"
        "                Value: {\n"
        "                    BinaryOperation: {\n"
        "                        Operator: +\n"
        "                        Lhs: {\n"
        "                            Number: 1\n"
        "                        }\n"
        "                        Rhs: {\n"
        "                            Number: 2\n"
        "                        }\n"
        "                    }\n"
        "                }\n"
        "            }\n"
        "            BinaryOperation: {\n"
        "                Operator: -=\n"
        "                Lhs: {\n"
        "                    Variable: b\n"
        "                }\n"
        "                Rhs: {\n"
        "                    Number: 2\n"
        "                }\n"
        "            }\n"
        + postfix);
}

TEST_CASE("Wrong parser files") {
    CHECK_THROWS_WITH(TryCreateParser("parser/WrongGlobal.ct"),
        "Failed to parse [Identifier 'foo' on line 2]. Expected Func.");
    CHECK_THROWS_WITH(TryCreateParser("parser/WrongFuncArg.ct"), "Failed to parse [Identifier 'a' on line 2]. Expected Var.");
    CHECK_THROWS_WITH(TryCreateParser("parser/WrongFuncComma.ct"), "Failed to parse [Bracket ')' on line 2]. Expected Var.");
    CHECK_THROWS_WITH(TryCreateParser("parser/WrongFuncBlock.ct"),
        "Failed to parse [Bracket '}' on line 3]. Expected Bracket.");
    CHECK_THROWS_WITH(TryCreateParser("parser/WrongStatement.ct"),
        "Failed to parse [Func 'func' on line 3]. Expected one of following { Return, For, If, While, Do, "
        "Binary Operator, Identifier, True, False, Number, String, Var, }.");
}

TEST_CASE("Function and variables types") {
    Parser p("examples/parser/ParserWithTypes.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() ==
        "Global: {\n"
        "    Symbols: { 10:ReadNumber, 11:ReadText, 6:Write, 8:WriteLine, 1:any, 3:bool, 12:foo, 5:number, 4:string, 2:void, }\n"
        "    FunctionDef: {\n"
        "        Name: foo\n"
        "        Symbols: { 13:a, 14:b, 15:v, }\n"
        "        Arguments: {\n"
        "            Definition: {\n"
        "                Variable: a\n"
        "                Type: number\n"
        "            }\n"
        "        }\n"
        "        Returns: {\n"
        "            Type: void\n"
        "        }\n"
        "        Block: {\n"
        "            Definition: {\n"
        "                Variable: b\n"
        "                Type: string\n"
        "            }\n"
        "            Definition: {\n"
        "                Variable: v\n"
        "                Type: bool\n"
        "                Value: {\n"
        "                    Bool: True\n"
        "                }\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}\n");
}

TEST_CASE("Range parser") {
    Parser p("examples/parser/ParserRange.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() == prefix() +
        "            For: {\n"
        "                Symbols: { 14:b, }\n"
        "                ControlVariable: {\n"
        "                    Definition: {\n"
        "                        Variable: b\n"
        "                        Type: any\n"
        "                    }\n"
        "                }\n"
        "                In: {\n"
        "                    From: {\n"
        "                        Number: 0\n"
        "                    }\n"
        "                    To: {\n"
        "                        Number: 3\n"
        "                    }\n"
        "                    IncludeLast: True\n"
        "                }\n"
        "                Block: {\n"
        "                }\n"
        "            }\n"
        + postfix);
}

} // namespace ParserTests
