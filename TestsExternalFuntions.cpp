#include "catch.hpp"

#include "Interpreter"

using namespace std;
using namespace Interpreter;

TEST_CASE("External functions") {
    SECTION("Write") {
        std::ostringstream oss;
        std::streambuf* buffer = std::cout.rdbuf();
        std::cout.rdbuf(oss.rdbuf());
        
        Parser p("examples/externalfunctions/Write.ct");
        INFO(p.Tree());
        p.Evaluate("foo");

        std::cout.rdbuf(buffer);
        CHECK(oss.str() == "hello world");
    }

    SECTION("Write with argument passed") {
        std::ostringstream oss;
        std::streambuf* buffer = std::cout.rdbuf();
        std::cout.rdbuf(oss.rdbuf());
        
        Parser p("examples/externalfunctions/WriteArgument.ct");
        INFO(p.Tree());
        p.Evaluate("foo", { "test"s });

        std::cout.rdbuf(buffer);
        CHECK(oss.str() == "hello test");
    }

    SECTION("WriteLine") {
        std::ostringstream oss;
        std::streambuf* buffer = std::cout.rdbuf();
        std::cout.rdbuf(oss.rdbuf());
        
        Parser p("examples/externalfunctions/WriteLine.ct");
        INFO(p.Tree());
        p.Evaluate("foo");

        std::cout.rdbuf(buffer);

        CHECK(oss.str() == "hello\nworld\n!!\n");
    }

    SECTION("WriteLine with argument passed") {
        std::ostringstream oss;
        std::streambuf* buffer = std::cout.rdbuf();
        std::cout.rdbuf(oss.rdbuf());
        
        Parser p("examples/externalfunctions/WriteLineArgument.ct");
        INFO(p.Tree());
        p.Evaluate("foo", { "test"s });

        std::cout.rdbuf(buffer);

        CHECK(oss.str() == "hello test\n!!\n");
    }

    SECTION("ReadText") {
        std::istringstream iss("test");
        std::streambuf* buffer = std::cin.rdbuf();
        std::cin.rdbuf(iss.rdbuf());
        
        Parser p("examples/externalfunctions/ReadText.ct");
        INFO(p.Tree());
        auto ret = std::get<string>(p.Evaluate("foo"));
        
        std::cin.rdbuf(buffer);
        CHECK(ret == "test");
    }

    SECTION("ReadNumber") {
        std::istringstream iss("1.5");
        std::streambuf* buffer = std::cin.rdbuf();
        std::cin.rdbuf(iss.rdbuf());
        
        Parser p("examples/externalfunctions/ReadNumber.ct");
        INFO(p.Tree());
        auto ret = std::get<double>(p.Evaluate("foo"));
        
        std::cin.rdbuf(buffer);
        CHECK(ret == 1.5);
    }
}