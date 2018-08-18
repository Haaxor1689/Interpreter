#include "Ast.hpp"

namespace Interpreter {

struct Logger {
    static void Created(const Node& node) {
        node.Print(std::cerr, 0);
        std::cerr << "--------------------" << std::endl;
    }
};

}