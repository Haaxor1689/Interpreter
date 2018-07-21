#include "Interpreter"

void Help() {
    std::cout << "Usage: InterpreterCLI {source file name} {function name} {arguments...}" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cerr << "Missing source file." << std::endl;
        Help();
        return -1;
    }
    
    if (argc <= 2) {
        std::cerr << "Missing function name to call." << std::endl;
        Help();
        return -1;
    }

    try {
        Interpreter::Parser parser(argv[1]);
        try {
            using Interpreter::operator<<;
            std::cout << "Evaluation returned: " << parser.Evaluate(argv[2], argc - 3, &argv[3]) << "." << std::endl;
        } catch(const std::exception& err) {
            std::cerr << "Failed to evaluate the provided function.\n" << err.what() << std::endl;
            return -1;
        }
    } catch(const std::exception& err) {
        std::cerr << "Failed to interpret the source file.\n" << err.what() << std::endl;
        return -1;
    }
    
    return 0;
}
