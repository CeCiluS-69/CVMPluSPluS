#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"
#include "VM.h"

void runCode(const std::string& source) {
    std::cout << "\n============================================\n";
    std::cout << "             CVM++ FRONTEND PIPELINE        \n";
    std::cout << "============================================\n";

    Lexer lexer;
    Parser parser;
    Compiler compiler;
    VM vm;

    auto tokens = lexer.tokenize(source);
    auto ast = parser.parse(tokens);
    auto bytecode = compiler.compile(ast);
    
    std::cout << "\n============================================\n";
    std::cout << "             VIRTUAL MACHINE RUNTIME        \n";
    std::cout << "============================================\n";
    vm.run(bytecode);
    std::cout << "============================================\n\n";
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        // File Runner Mode
        std::ifstream file(argv[1]);
        if (!file.is_open()) {
            std::cerr << "Could not open file." << std::endl;
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        runCode(buffer.str());
    } else {
        // REPL Mode
        std::string line;
        std::cout << "CVM++ REPL v1.0 (Type 'exit' to quit)\n";
        while (true) {
            std::cout << "cvm> ";
            std::getline(std::cin, line);
            if (line == "exit") break;
            if (!line.empty()) {
                runCode(line);
            }
        }
    }
    return 0;
}
