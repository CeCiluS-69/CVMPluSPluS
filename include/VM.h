#pragma once
#include "Common.h"

class VM {
    // Fast fixed-size arrays completely bypass std::vector allocation/bounds overhead
    int stack[1024]; 
    int sp = 0; // Stack Pointer
    int variables[256] = {0}; 

    // Inline pushing and popping directly to the array
    inline void push(int val) { stack[sp++] = val; }
    inline int pop() { return stack[--sp]; }

public:
    void run(const std::vector<uint8_t>& bytecode) {
        size_t ip = 0; 

        // Infinite loop avoids checking ip < bytecode.size() every cycle
        for (;;) {
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
                    return; // Replaces loop check
  
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
