#include "catch.hpp"

#include "Interpreter"

using namespace std;
using namespace Interpreter;

namespace {

void TryCreateParser(const std::string& name) {
    Parser p("examples/" + name);
}

} // namespace

TEST_CASE("Function call") {
    SECTION("Function return") {
        CHECK_NOTHROW(TryCreateParser("typecheck/VoidReturn.ct"));
        CHECK_THROWS_WITH(TryCreateParser("typecheck/ArgumentReturn.ct"),
                          "Type mismatch error. Expected \"number\" got \"bool\".");
        CHECK_THROWS_WITH(TryCreateParser("typecheck/MissingReturn.ct"),
                          "Type mismatch error. Expected \"bool\" got \"void\".");
        CHECK_THROWS_WITH(TryCreateParser("typecheck/WrongReturn.ct"),
                          "Type mismatch error. Expected \"number\" got \"bool\".");
        CHECK_THROWS_WITH(TryCreateParser("typecheck/WrongMultipleReturn.ct"),
                          "Type mismatch error. Expected \"void\" got \"number\".");
    }

    SECTION("AnyType") {
        CHECK_THROWS_WITH(TryCreateParser("typecheck/ReassignAny.ct"),
                          "Type mismatch error. Expected \"number\" got \"string\".");
    }
}