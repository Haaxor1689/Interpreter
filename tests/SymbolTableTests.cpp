#include "catch.hpp"

#include "../Interpreter"

using namespace std;
using namespace Interpreter;

namespace {

void TryCreateParser(const std::string& name) {
    Parser p("examples/" + name);
}

} // namespace

namespace SymbolTableTests {

TEST_CASE("Function names") {
    Parser p("examples/symbols/FunctionDef.ct");
    INFO(p.Tree());
    CHECK(p.Tree().Root().Symbols()["foo"].id == 12);
    CHECK(p.Tree().Root().Symbols()["goo"].id == 13);
}

TEST_CASE("Undefined symbol exception") {
    CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowUndefinedFunc.ct"), "An exception occured on line 6. Message: Found undefined identifier a.");
    CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowForLoopIdentifierOutside.ct"), "An exception occured on line 8. Message: Found undefined identifier a.");
    CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowSymbolFromAnotherBlock.ct"), "An exception occured on line 6. Message: Found undefined identifier a.");
    CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowUseBeforeDeclaration.ct"), "An exception occured on line 6. Message: Found undefined identifier a.");
    CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowOnRedefinition.ct"), "An exception occured on line 4. Message: Tried to redefine identifier a.");
    CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowUndefinedObjectAttribute.ct"), "An exception occured on line 5. Message: Found undefined identifier b.");
}

} // namespace SymbolTableTests
