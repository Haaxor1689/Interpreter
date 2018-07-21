#include "catch.hpp"

#include "Interpreter"

using namespace std;
using namespace Interpreter;

namespace {

void TryCreateParser(const std::string& name) {
    Parser p("examples/" + name);
}

} // namespace

TEST_CASE("Typecheck") {
    SECTION("Function return") {
        CHECK_NOTHROW(TryCreateParser("typecheck/VoidReturn.ct"));
        CHECK_THROWS_WITH(TryCreateParser("typecheck/ArgumentReturn.ct"),
                          "Type mismatch error on line 7. Expected \"number\" got \"bool\".");
        CHECK_THROWS_WITH(TryCreateParser("typecheck/MissingReturn.ct"),
                          "Type mismatch error on line 1 caused by wrong return type. Expected \"bool\" got \"void\".");
        CHECK_THROWS_WITH(TryCreateParser("typecheck/WrongReturn.ct"),
                          "Type mismatch error on line 1 caused by wrong return type. Expected \"number\" got \"bool\".");
        CHECK_THROWS_WITH(TryCreateParser("typecheck/WrongMultipleReturn.ct"),
                          "Type mismatch error on line 2 caused wrong return type. Expected \"void\" got \"number\".");
    }

    SECTION("AnyType") {
        CHECK_THROWS_WITH(TryCreateParser("typecheck/ReassignAny.ct"),
                          "Type mismatch error on line 3. Expected \"number\" got \"string\".");
    }

    SECTION("Function arguments") {
        CHECK_THROWS_WITH(TryCreateParser("typecheck/WrongArguments.ct"),
                          "Type mismatch error on line 5. Expected \"string\" got \"number\".");
    }
}