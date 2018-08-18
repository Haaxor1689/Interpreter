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
    return "Global: {\n"
           "    Symbols: { 10:ReadNumber, 11:ReadText, 6:Write, 8:WriteLine, 1:any, 3:bool, 12:foo, 5:number, 4:string, 2:void, }\n"
           "    FunctionDef: {\n"
           "        Name: 12:foo\n"
           "        Symbols: { 13:a," + additionalSymbols + " }\n"
           "        Arguments: {\n"
           "            Definition: {\n"
           "                Variable: 13:a\n"
           "                Type: 1:any\n"
           "            }\n"
           "        }\n"
           "        Returns: {\n"
           "            Type: 2:void\n"
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
        "                Variable: 14:bbb\n"
        "                Type: 1:any\n"
        "            }\n"
        "            Definition: {\n"
        "                Variable: 15:yyy\n"
        "                Type: 1:any\n"
        "            }\n"
        "            Definition: {\n"
        "                Variable: 16:ccc\n"
        "                Type: 1:any\n"
        "            }\n"
        "            For: {\n"
        "                Symbols: { 17:aaa, }\n"
        "                ControlVariable: {\n"
        "                    Definition: {\n"
        "                        Variable: 17:aaa\n"
        "                        Type: 1:any\n"
        "                    }\n"
        "                }\n"
        "                In: {\n"
        "                    Variable: 14:bbb\n"
        "                }\n"
        "                Block: {\n"
        "                    For: {\n"
        "                        Symbols: { 18:xxx, }\n"
        "                        ControlVariable: {\n"
        "                            Definition: {\n"
        "                                Variable: 18:xxx\n"
        "                                Type: 1:any\n"
        "                            }\n"
        "                        }\n"
        "                        In: {\n"
        "                            Variable: 15:yyy\n"
        "                        }\n"
        "                        Block: {\n"
        "                            Variable: 16:ccc\n"
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
        "            Variable: {\n"
        "                Name: 12:foo\n"
        "                Call: {\n"
        "                    Arguments: {\n"
        "                        Variable: 13:a\n"
        "                    }\n"
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
        "                    Variable: 13:a\n"
        "                }\n"
        "                Block: {\n"
        "                    Variable: 13:a\n"
        "                }\n"
        "            }\n"
        "            Elseif: {\n"
        "                Symbols: { 14:b, }\n"
        "                Condition: {\n"
        "                    Definition: {\n"
        "                        Variable: 14:b\n"
        "                        Type: 1:any\n"
        "                    }\n"
        "                }\n"
        "                Block: {\n"
        "                    Variable: 14:b\n"
        "                }\n"
        "            }\n"
        "            Else: {\n"
        "                Symbols: { }\n"
        "                Block: {\n"
        "                    Variable: 13:a\n"
        "                }\n"
        "            }\n"
        "            Variable: 13:a\n"
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
        "                    Variable: 13:a\n"
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
        "                    Variable: 13:a\n"
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
        "                    Variable: 13:a\n"
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
        "        Name: 12:goo\n"
        "        Symbols: { }\n"
        "        Arguments: {\n"
        "        }\n"
        "        Returns: {\n"
        "            Type: 2:void\n"
        "        }\n"
        "        Block: {\n"
        "        }\n"
        "    }\n"
        "    FunctionDef: {\n"
        "        Name: 13:foo\n"
        "        Symbols: { 14:a, 15:b, 16:c, }\n"
        "        Arguments: {\n"
        "            Definition: {\n"
        "                Variable: 14:a\n"
        "                Type: 1:any\n"
        "            }\n"
        "        }\n"
        "        Returns: {\n"
        "            Type: 2:void\n"
        "        }\n"
        "        Block: {\n"
        "            Definition: {\n"
        "                Variable: 15:b\n"
        "                Type: 1:any\n"
        "                Value: {\n"
        "                    Variable: 14:a\n"
        "                }\n"
        "            }\n"
        "            Definition: {\n"
        "                Variable: 16:c\n"
        "                Type: 2:void\n"
        "            }\n"
        "            Variable: {\n"
        "                Name: 16:c\n"
        "                Assignment: {\n"
        "                    Variable: {\n"
        "                        Name: 12:goo\n"
        "                        Call: {\n"
        "                            Arguments: {\n"
        "                            }\n"
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
        "                Variable: 14:b\n"
        "                Type: 5:number\n"
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
        "                    Variable: 14:b\n"
        "                }\n"
        "                Rhs: {\n"
        "                    Number: 2\n"
        "                }\n"
        "            }\n"
        + postfix);
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

TEST_CASE("Function and variables types") {
    Parser p("examples/parser/ParserWithTypes.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() ==
        "Global: {\n"
        "    Symbols: { 10:ReadNumber, 11:ReadText, 6:Write, 8:WriteLine, 1:any, 3:bool, 12:foo, 5:number, 4:string, 2:void, }\n"
        "    FunctionDef: {\n"
        "        Name: 12:foo\n"
        "        Symbols: { 13:a, 14:b, 15:v, }\n"
        "        Arguments: {\n"
        "            Definition: {\n"
        "                Variable: 13:a\n"
        "                Type: 5:number\n"
        "            }\n"
        "        }\n"
        "        Returns: {\n"
        "            Type: 2:void\n"
        "        }\n"
        "        Block: {\n"
        "            Definition: {\n"
        "                Variable: 14:b\n"
        "                Type: 4:string\n"
        "            }\n"
        "            Definition: {\n"
        "                Variable: 15:v\n"
        "                Type: 3:bool\n"
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
        "                        Variable: 14:b\n"
        "                        Type: 1:any\n"
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

TEST_CASE("Object parser") {
    Parser p("examples/parser/ParserObjectDef.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() == 
        "Global: {\n"
        "    Symbols: { 12:Goo, 10:ReadNumber, 11:ReadText, 6:Write, 8:WriteLine, 1:any, 3:bool, 5:number, 4:string, 2:void, }\n"
        "    Object: {\n"
        "        Name: 12:Goo\n"
        "        Symbols: { 13:a, 14:b, 15:c, }\n"
        "        Attributes: {\n"
        "            Definition: {\n"
        "                Variable: 13:a\n"
        "                Type: 1:any\n"
        "            }\n"
        "            Definition: {\n"
        "                Variable: 14:b\n"
        "                Type: 5:number\n"
        "                Value: {\n"
        "                    Number: 10\n"
        "                }\n"
        "            }\n"
        "            Definition: {\n"
        "                Variable: 15:c\n"
        "                Type: 4:string\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}\n");
}

TEST_CASE("Object init parser") {
    Parser p("examples/parser/ParserObjectInit.ct");
    ostringstream oss;
    oss << p.Tree();
    CHECK(oss.str() == 
        "Global: {\n"
        "    Symbols: { 16:Foo, 12:Goo, 10:ReadNumber, 11:ReadText, 6:Write, 8:WriteLine, 1:any, 3:bool, 5:number, 4:string, 2:void, }\n"
        "    Object: {\n"
        "        Name: 12:Goo\n"
        "        Symbols: { 13:a, 14:b, 15:c, }\n"
        "        Attributes: {\n"
        "            Definition: {\n"
        "                Variable: 13:a\n"
        "                Type: 1:any\n"
        "            }\n"
        "            Definition: {\n"
        "                Variable: 14:b\n"
        "                Type: 5:number\n"
        "                Value: {\n"
        "                    Number: 10\n"
        "                }\n"
        "            }\n"
        "            Definition: {\n"
        "                Variable: 15:c\n"
        "                Type: 4:string\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "    FunctionDef: {\n"
        "        Name: 16:Foo\n"
        "        Symbols: { 17:obj, }\n"
        "        Arguments: {\n"
        "        }\n"
        "        Returns: {\n"
        "            Type: 2:void\n"
        "        }\n"
        "        Block: {\n"
        "            Definition: {\n"
        "                Variable: 17:obj\n"
        "                Type: 12:Goo\n"
        "                Value: {\n"
        "                    ObjectInit: {\n"
        "                        Type: 12:Goo\n"
        "                        a: {\n"
        "                            String: \"a\"\n"
        "                        }\n"
        "                        c: {\n"
        "                            String: \"c\"\n"
        "                        }\n"
        "                    }\n"
        "                }\n"
        "            }\n"
        "        }\n"
        "    }\n"
        "}\n");
}

} // namespace ParserTests
