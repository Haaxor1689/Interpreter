#include "catch.hpp"

#include <sstream>

#include "Interpreter"

using namespace std;
using namespace Interpreter;

namespace {

void TryCreateParser(const std::string& name) {
    Parser p("examples/" + name);
}

} // namespace

TEST_CASE("Parser") {
    SECTION("Simple parser test") {
        Parser p("examples/parser/ParserSimple.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 2:bool, 5:goo, 4:number, 3:string, 1:void, }\n"
              "    FunctionDef: {\n"
              "        Name: goo\n"
              "        Symbols: { 7:boo, 6:foo, }\n"
              "        Arguments: {\n"
              "            Definition: {\n"
              "                Variable: foo\n"
              "            }\n"
              "            Definition: {\n"
              "                Variable: boo\n"
              "            }\n"
              "        }\n"
              "        Block: {\n"
              "            String: \"a\"\n"
              "            Double: 1\n"
              "        }\n"
              "    }\n"
              "}\n");
    }

    SECTION("For loop parsing") {
        Parser p("examples/parser/ParserForLoop.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 2:bool, 5:goo, 4:number, 3:string, 1:void, }\n"
              "    FunctionDef: {\n"
              "        Name: goo\n"
              "        Symbols: { 6:bbb, 8:ccc, 7:yyy, }\n"
              "        Arguments: {\n"
              "            Definition: {\n"
              "                Variable: bbb\n"
              "            }\n"
              "            Definition: {\n"
              "                Variable: yyy\n"
              "            }\n"
              "            Definition: {\n"
              "                Variable: ccc\n"
              "            }\n"
              "        }\n"
              "        Block: {\n"
              "            For: {\n"
              "                Symbols: { 9:aaa, }\n"
              "                Definition: {\n"
              "                    Variable: aaa\n"
              "                }\n"
              "                Range: {\n"
              "                    Variable: bbb\n"
              "                }\n"
              "                Block: {\n"
              "                    For: {\n"
              "                        Symbols: { 10:xxx, }\n"
              "                        Definition: {\n"
              "                            Variable: xxx\n"
              "                        }\n"
              "                        Range: {\n"
              "                            Variable: yyy\n"
              "                        }\n"
              "                        Block: {\n"
              "                            Variable: ccc\n"
              "                        }\n"
              "                    }\n"
              "                }\n"
              "            }\n"
              "        }\n"
              "    }\n"
              "}\n");
    }

    SECTION("Function call parsing") {
        Parser p("examples/parser/ParserFunctionCall.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 2:bool, 5:goo, 4:number, 3:string, 1:void, }\n"
              "    FunctionDef: {\n"
              "        Name: goo\n"
              "        Symbols: { 6:a, }\n"
              "        Arguments: {\n"
              "            Definition: {\n"
              "                Variable: a\n"
              "            }\n"
              "        }\n"
              "        Block: {\n"
              "            FunctionCall: {\n"
              "                Name: goo\n"
              "                Arguments: {\n"
              "                    Variable: a\n"
              "                }\n"
              "            }\n"
              "        }\n"
              "    }\n"
              "}\n");
    }

    SECTION("If/Else parsing") {
        Parser p("examples/parser/ParserIfElse.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 2:bool, 5:foo, 4:number, 3:string, 1:void, }\n"
              "    FunctionDef: {\n"
              "        Name: foo\n"
              "        Symbols: { 6:a, }\n"
              "        Arguments: {\n"
              "            Definition: {\n"
              "                Variable: a\n"
              "            }\n"
              "        }\n"
              "        Block: {\n"
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
              "                Symbols: { 7:b, }\n"
              "                Condition: {\n"
              "                    Definition: {\n"
              "                        Variable: b\n"
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
              "        }\n"
              "    }\n"
              "}\n");
    }

    SECTION("If without else") {
        Parser p("examples/parser/ParserIf.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 2:bool, 5:foo, 4:number, 3:string, 1:void, }\n"
              "    FunctionDef: {\n"
              "        Name: foo\n"
              "        Symbols: { 6:a, }\n"
              "        Arguments: {\n"
              "            Definition: {\n"
              "                Variable: a\n"
              "            }\n"
              "        }\n"
              "        Block: {\n"
              "            If: {\n"
              "                Symbols: { }\n"
              "                Condition: {\n"
              "                    Variable: a\n"
              "                }\n"
              "                Block: {\n"
              "                }\n"
              "            }\n"
              "        }\n"
              "    }\n"
              "}\n");
    }

    SECTION("While parsing") {
        Parser p("examples/parser/ParserWhile.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 2:bool, 5:foo, 4:number, 3:string, 1:void, }\n"
              "    FunctionDef: {\n"
              "        Name: foo\n"
              "        Symbols: { 6:a, }\n"
              "        Arguments: {\n"
              "            Definition: {\n"
              "                Variable: a\n"
              "            }\n"
              "        }\n"
              "        Block: {\n"
              "            While: {\n"
              "                Symbols: { }\n"
              "                Condition: {\n"
              "                    Variable: a\n"
              "                }\n"
              "                Block: {\n"
              "                }\n"
              "            }\n"
              "        }\n"
              "    }\n"
              "}\n");
    }

    SECTION("DoWhile parsing") {
        Parser p("examples/parser/ParserDoWhile.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 2:bool, 5:foo, 4:number, 3:string, 1:void, }\n"
              "    FunctionDef: {\n"
              "        Name: foo\n"
              "        Symbols: { 6:a, }\n"
              "        Arguments: {\n"
              "            Definition: {\n"
              "                Variable: a\n"
              "            }\n"
              "        }\n"
              "        Block: {\n"
              "            DoWhile: {\n"
              "                Symbols: { }\n"
              "                Condition: {\n"
              "                    Variable: a\n"
              "                }\n"
              "                Block: {\n"
              "                }\n"
              "            }\n"
              "        }\n"
              "    }\n"
              "}\n");
    }

    SECTION("Assignment parsing") {
        Parser p("examples/parser/ParserAssignment.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 2:bool, 6:foo, 5:goo, 4:number, 3:string, 1:void, }\n"
              "    FunctionDef: {\n"
              "        Name: goo\n"
              "        Symbols: { }\n"
              "        Arguments: {\n"
              "        }\n"
              "        Block: {\n"
              "        }\n"
              "    }\n"
              "    FunctionDef: {\n"
              "        Name: foo\n"
              "        Symbols: { 7:a, 8:b, 9:c, }\n"
              "        Arguments: {\n"
              "            Definition: {\n"
              "                Variable: a\n"
              "            }\n"
              "        }\n"
              "        Block: {\n"
              "            Definition: {\n"
              "                Variable: b\n"
              "                Value: {\n"
              "                    Variable: a\n"
              "                }\n"
              "            }\n"
              "            Definition: {\n"
              "                Variable: c\n"
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

    SECTION("Operators parser") {
        Parser p("examples/parser/ParserOperators.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 2:bool, 5:foo, 4:number, 3:string, 1:void, }\n"
              "    FunctionDef: {\n"
              "        Name: foo\n"
              "        Symbols: { 6:a, }\n"
              "        Arguments: {\n"
              "        }\n"
              "        Block: {\n"
              "            Definition: {\n"
              "                Variable: a\n"
              "                Value: {\n"
              "                    BinaryOperation: {\n"
              "                        Operator: +\n"
              "                        Lhs: {\n"
              "                            Double: 1\n"
              "                        }\n"
              "                        Rhs: {\n"
              "                            Double: 2\n"
              "                        }\n"
              "                    }\n"
              "                }\n"
              "            }\n"
              "            BinaryOperation: {\n"
              "                Operator: -=\n"
              "                Lhs: {\n"
              "                    Variable: a\n"
              "                }\n"
              "                Rhs: {\n"
              "                    Double: 2\n"
              "                }\n"
              "            }\n"
              "        }\n"
              "    }\n"
              "}\n");
    }

    SECTION("Wrong parser files") {
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

    SECTION("Function and variables types") {
        Parser p("examples/parser/ParserWithTypes.ct");
        ostringstream oss;
        oss << p.Tree();
        CHECK(oss.str() ==
              "Global: {\n"
              "    Symbols: { 2:bool, 5:foo, 4:number, 3:string, 1:void, }\n"
              "    FunctionDef: {\n"
              "        Name: foo\n"
              "        Symbols: { 6:a, 7:b, 8:v, }\n"
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
              "                    Bool: true\n"
              "                }\n"
              "            }\n"
              "        }\n"
              "    }\n"
              "}\n");
    }
}