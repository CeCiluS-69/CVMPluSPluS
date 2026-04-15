#pragma once
#include "Common.h"

class VM {
    std::vector<int> stack;
    int variables[256] = {0}; // Memory for variables

    void push(int val) { stack.push_back(val); }

    int pop() {
        if (stack.empty()) {
            std::cerr << "Runtime Error: Stack underflow!" << std::endl;
            return 0;
        }
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

                case OP_SUB: {
                    int b = pop();
                    int a = pop();
                    push(a - b);
                    break;
                }

                case OP_MUL: {
                    int b = pop();
                    int a = pop();
                    push(a * b);
                    break;
                }

                case OP_DIV: {
                    int b = pop();
                    int a = pop();

                    if (b == 0) {
                        std::cerr << "Runtime Error: Division by zero!" << std::endl;
                        return;
                    }

                    push(a / b);
                    break;
                }

                case OP_LESS: {
                    int b = pop();
                    int a = pop();
                    push(a < b); 
                    break;
                }

                case OP_GREATER: {
                    int b = pop();
                    int a = pop();
                    push(a > b);
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
  
                case OP_JUMP: {
                    ip = bytecode[ip];
                    break;
                }

                case OP_JUMP_IF_FALSE: {
                    int cond = pop();
                    uint8_t target = bytecode[ip++];
                    if (!cond) ip = target;
                    break;
                }

                default:
                    std::cerr << "Runtime Error: Unknown opcode " << (int)instruction << std::endl;
                    return;
            }
        }
    }
};
