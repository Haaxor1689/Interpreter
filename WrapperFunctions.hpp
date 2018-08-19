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
        std::string read;
        std::getline(std::cin, read);
        try {
            return std::stod(read);
        } catch (const std::invalid_argument&) {
            throw InternalException("Failed to convert input to number.");
        }
    }

    std::string ReadText() {
        std::string read;
        std::getline(std::cin, read);
        return read;
    }
}
