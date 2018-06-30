#include <string>
#include <iostream>

namespace Interpreter {
    void Write(const std::string& string) {
        std::cout << string;
    }

    void WriteLine(const std::string& string) {
        std::cout << string << std::endl;
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
