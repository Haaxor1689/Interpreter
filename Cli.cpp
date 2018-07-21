#include "Interpreter"
#include <iostream>

std::string GetCommandDescription(const std::string& name, const std::string& usage, const std::string& description) {
    return name + ":\n    Usage: InterpreterCLI " + usage + "\n    Description: " + description + "\n";
}

void Help() {
    std::cout << "==== Commands: ====\n" 
        << GetCommandDescription("Help", "-h", "Prints help.")
        << GetCommandDescription("Tree", "-tree {source file name}", "Prints out ast tree of provided source file.")
        << GetCommandDescription("Evaluate", "-eval {source file name} {function name} {function arguments...}", "Evaluates a function from source file with provided arguments.");
}

int Evaluate(int argc, char* argv[]) {
    if (argc <= 0) {
        std::cerr << "Missing source file." << std::endl;
        Help();
        return -1;
    }

    if (argc <= 1) {
        std::cerr << "Missing function name to call." << std::endl;
        Help();
        return -1;
    }

    try {
        Interpreter::Parser parser(argv[0]);
        try {
            using Interpreter::operator<<;
            std::cout << "Evaluation returned: " << parser.Evaluate(argv[1], argc - 2, &argv[2]) << "." << std::endl;
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

int Tree(int argc, char* argv[]) {
    if (argc <= 0) {
        std::cerr << "Missing source file." << std::endl;
        Help();
        return -1;
    }

    try {
        Interpreter::Parser parser(argv[0]);
        std::cout << parser.Tree();
    } catch(const std::exception& err) {
        std::cerr << "Failed to interpret the source file.\n" << err.what() << std::endl;
        return -1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cerr << "Missing command to execute." << std::endl;
        Help();
        return -1;
    }

    std::string command(argv[1]);
    if (command == "-tree") {
        return Tree(argc - 2, &argv[2]);
    }
    
    if (command == "-eval") {
        return Evaluate(argc - 2, &argv[2]);
    }

    if (command == "-help") {
        Help();
        return 0;
    }

    std::cerr << "Unknown command." << std::endl;
    Help();
    return 0;
}
