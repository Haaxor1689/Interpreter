#include "catch.hpp"

#include "../Interpreter"
#include <iostream>

using namespace std;
using namespace Interpreter;

namespace ExternalFunctionsTests {

TEST_CASE("Write") {
    std::ostringstream oss;
    std::streambuf* buffer = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());

    Parser p("examples/externalfunctions/Write.ct");
    INFO(p.Tree());
    p.Evaluate("foo");

    std::cout.rdbuf(buffer);
    CHECK(oss.str() == "hello world");
}

TEST_CASE("Write with argument passed") {
    std::ostringstream oss;
    std::streambuf* buffer = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());

    Parser p("examples/externalfunctions/WriteArgument.ct");
    INFO(p.Tree());
    p.Evaluate("foo", { "test"s });

    std::cout.rdbuf(buffer);
    CHECK(oss.str() == "hello test");
}

TEST_CASE("WriteLine") {
    std::ostringstream oss;
    std::streambuf* buffer = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());

    Parser p("examples/externalfunctions/WriteLine.ct");
    INFO(p.Tree());
    p.Evaluate("foo");

    std::cout.rdbuf(buffer);

    CHECK(oss.str() == "hello\nworld\n!!\n");
}

TEST_CASE("WriteLine with argument passed") {
    std::ostringstream oss;
    std::streambuf* buffer = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());

    Parser p("examples/externalfunctions/WriteLineArgument.ct");
    INFO(p.Tree());
    p.Evaluate("foo", { "test"s });

    std::cout.rdbuf(buffer);

    CHECK(oss.str() == "hello test\n!!\n");
}

TEST_CASE("ReadText") {
    std::istringstream iss("test");
    std::streambuf* buffer = std::cin.rdbuf();
    std::cin.rdbuf(iss.rdbuf());

    Parser p("examples/externalfunctions/ReadText.ct");
    INFO(p.Tree());
    auto ret = std::get<string>(p.Evaluate("foo"));

    std::cin.rdbuf(buffer);
    CHECK(ret == "test");
}

TEST_CASE("ReadNumber") {
    std::istringstream iss("1.5");
    std::streambuf* buffer = std::cin.rdbuf();
    std::cin.rdbuf(iss.rdbuf());

    Parser p("examples/externalfunctions/ReadNumber.ct");
    INFO(p.Tree());
    auto ret = std::get<double>(p.Evaluate("foo"));

    std::cin.rdbuf(buffer);
    CHECK(ret == 1.5);
}

} // namespace ExternalFunctionsTests
