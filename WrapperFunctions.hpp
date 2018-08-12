#pragma once

#include <string>
#include <iostream>

#include "Helpers.hpp"

namespace Interpreter {
    void Write(const Value& string) {
        std::cout << ToString(string);
    }

    void WriteLine(const Value& string) {
        std::cout << ToString(string) << std::endl;
    }

    double ReadNumber() {
        double read;
        std::cin >> read;
        return read;
    }

    std::string ReadText() {
        std::string read;
        std::cin >> read;
        return read;
    }
}
