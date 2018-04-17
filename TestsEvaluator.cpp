#include "catch.hpp"

#include <sstream>

#include "Interpreter"

using namespace std;
using namespace Interpreter;

TEST_CASE("Evaluator") {
    SECTION("Simple functions") {
        Parser p("examples/evaluator/Empty.ct");
        auto retP = p.Evaluate("foo", {});
        ostringstream oss;
        oss << retP;
        CHECK(oss.str() == "Null");

        Parser q("examples/evaluator/BoolReturn.ct");
        auto retQ = std::get<bool>(q.Evaluate("foo", {}));
        CHECK(retQ == true);

        Parser r("examples/evaluator/DoubleReturn.ct");
        auto retR = std::get<double>(r.Evaluate("foo", {}));
        CHECK(retR == 12.4);

        Parser s("examples/evaluator/StringReturn.ct");
        auto retS = std::get<std::string>(s.Evaluate("foo", {}));
        CHECK(retS == "Test");
    }
}