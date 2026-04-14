#pragma once
#include "Common.h"

class VM {
    std::vector<int> stack;
    int variables[256] = {0}; // Simple memory array for 256 variables

    void push(int val) { stack.push_back(val); }
    int pop() { 
        int val = stack.back(); 
        stack.pop_back(); 
        return val; 
    }

public:
    void run(const std::vector<uint8_t>& bytecode) {
        size_t ip = 0; // Instruction Pointer

        while (ip < bytecode.size()) {
            uint8_t instruction = bytecode[ip++];

            switch (instruction) {
                case OP_PUSH: {
                    push(bytecode[ip++]);
                    break;
                }
                case OP_ADD: {
                    int b = pop();
                    int a = pop();
                    push(a + b);
                    break;
                }
                case OP_STORE: {
                    uint8_t slot = bytecode[ip++];
                    variables[slot] = pop();
                    break;
                }
                case OP_LOAD: {
                    uint8_t slot = bytecode[ip++];
                    push(variables[slot]);
                    break;
                }
                case OP_PRINT: {
                    std::cout << pop() << std::endl;
                    break;
                }
                case OP_HALT:
                    return;
            }
        }
    }
};
