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
    CHECK(p.Tree().Root().Symbols().GetSymbol("foo").id == 12);
    CHECK(p.Tree().Root().Symbols().GetSymbol("goo").id == 13);
}

TEST_CASE("Undefined symbol exception") {
    CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowUndefinedFunc.ct"), "Found undefined identifier a.");
    CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowForLoopIdentifierOutside.ct"), "Found undefined identifier a.");
    CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowSymbolFromAnotherBlock.ct"), "Found undefined identifier a.");
    CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowUseBeforeDeclaration.ct"), "Found undefined identifier a.");
    CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowOnRedefinition.ct"), "Tried to redefine identifier a.");
    CHECK_THROWS_WITH(TryCreateParser("symbols/ThrowUndefinedObjectAttribute.ct"), "Found undefined identifier b.");
}

} // namespace SymbolTableTests
