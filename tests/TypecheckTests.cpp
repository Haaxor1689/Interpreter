#include "catch.hpp"

#include "../Interpreter"

using namespace std;
using namespace Interpreter;

namespace {

void TryCreateParser(const std::string& name) {
    Parser p("examples/" + name);
}

} // namespace

namespace TypecheckTests {

TEST_CASE("Function return") {
    CHECK_NOTHROW(TryCreateParser("typecheck/VoidReturn.ct"));
    CHECK_THROWS_WITH(TryCreateParser("typecheck/ArgumentReturn.ct"),
        "Type mismatch error on line 7. Expected \"5:number\" got \"3:bool\".");
    CHECK_THROWS_WITH(TryCreateParser("typecheck/MissingReturn.ct"),
        "Type mismatch error on line 1 caused by wrong return type. Expected \"3:bool\" got \"2:void\".");
    CHECK_THROWS_WITH(TryCreateParser("typecheck/WrongReturn.ct"),
        "Type mismatch error on line 1 caused by wrong return type. Expected \"5:number\" got \"3:bool\".");
    CHECK_THROWS_WITH(TryCreateParser("typecheck/WrongMultipleReturn.ct"),
        "Type mismatch error on line 5 caused by wrong return type. Expected \"5:number\" got \"4:string\".");
}

TEST_CASE("AnyType") {
    CHECK_THROWS_WITH(TryCreateParser("typecheck/ReassignAny.ct"),
        "Type mismatch error on line 3. Expected \"5:number\" got \"4:string\".");
}

TEST_CASE("Function arguments") {
    CHECK_THROWS_WITH(TryCreateParser("typecheck/WrongArguments.ct"),
        "Type mismatch error on line 5. Expected \"4:string\" got \"5:number\".");
}

TEST_CASE("Object typecheck") {
    CHECK_NOTHROW(TryCreateParser("typecheck/CorrectObject.ct"));
    CHECK_THROWS_WITH(TryCreateParser("typecheck/WrongObjectDef.ct"),
        "Type mismatch error on line 2. Expected \"5:number\" got \"4:string\".");
    CHECK_THROWS_WITH(TryCreateParser("typecheck/WrongObjectInit.ct"),
        "Type mismatch error on line 7 caused by missing 14:b of type 1:any. Expected \"12:Goo\" got \"Unknown object\".");
    CHECK_THROWS_WITH(TryCreateParser("typecheck/WrongObjectInit2.ct"),
        "Type mismatch error on line 1. Expected \"5:number\" got \"4:string\".");
    CHECK_THROWS_WITH(TryCreateParser("typecheck/WrongObjectAttr.ct"),
        "Type mismatch error on line 5 caused by wrong return type. Expected \"5:number\" got \"4:string\".");
}

TEST_CASE("Dot operator chaining") {
    CHECK_NOTHROW(TryCreateParser("typecheck/DotOperatorChaining.ct"));
}

} // namespace TypecheckTests
