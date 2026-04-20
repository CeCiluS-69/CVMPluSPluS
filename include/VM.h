#pragma once
#include "Common.h"
#include <iomanip> 
#include <unordered_map>

// ==========================================
// 🔹 BRANCH PREDICTOR (BTB & BPB)
// ==========================================
enum PredictorState {
    STRONGLY_NOT_TAKEN = 0, // 00
    WEAKLY_NOT_TAKEN   = 1, // 01
    WEAKLY_TAKEN       = 2, // 10
    STRONGLY_TAKEN     = 3  // 11
};

struct BranchEntry {
    size_t targetAddress;       // BTB: Target Instruction Pointer
    PredictorState state;       // BPB: 2-bit Saturating Counter
    BranchEntry() : targetAddress(0), state(WEAKLY_NOT_TAKEN) {}
};

class BranchPredictor {
public:
    std::unordered_map<size_t, BranchEntry> table;
    int correct = 0;
    int total = 0;

    bool predict(size_t pc, size_t& predictedTarget) {
        if (table.find(pc) == table.end()) return false; 
        
        BranchEntry& entry = table[pc];
        bool isTaken = (entry.state == WEAKLY_TAKEN || entry.state == STRONGLY_TAKEN);
        
        if (isTaken) predictedTarget = entry.targetAddress;
        return isTaken;
    }

    void update(size_t pc, size_t actualTarget, bool actuallyTaken, bool predictedTaken) {
        total++;
        if (actuallyTaken == predictedTaken) correct++;

        if (table.find(pc) == table.end()) table[pc] = BranchEntry();

        BranchEntry& entry = table[pc];
        
        if (actuallyTaken) entry.targetAddress = actualTarget;

        if (actuallyTaken) {
            if (entry.state != STRONGLY_TAKEN) 
                entry.state = static_cast<PredictorState>(entry.state + 1);
        } else {
            if (entry.state != STRONGLY_NOT_TAKEN) 
                entry.state = static_cast<PredictorState>(entry.state - 1);
        }
    }

    void printStats() {
        std::cout << "\n--- Hardware Simulation Stats ---\n";
        std::cout << "Branch Predictions: " << total << "\n";
        std::cout << "Correct Guesses:    " << correct << "\n";
        if (total > 0) {
            std::cout << "Accuracy:           " << std::fixed << std::setprecision(2) 
                      << ((float)correct / total * 100.0) << "%\n";
        }
    }
};

// ==========================================
// 🔹 VIRTUAL MACHINE
// ==========================================
class VM {
    // Fast fixed-size arrays completely bypass std::vector allocation/bounds overhead
    int stack[1024]; 
    int sp = 0; // Stack Pointer
    int variables[256] = {0}; 
    
    // Hardware simulation state
    BranchPredictor predictor;
    size_t simulatedCycles = 0;

    // Inline pushing and popping directly to the array
    inline void push(int val) { stack[sp++] = val; }
    inline int pop() { return stack[--sp]; }

public:
    void run(const std::vector<uint8_t>& bytecode) {
        size_t ip = 0; 

        // Infinite loop avoids checking ip < bytecode.size() every cycle
        for (;;) {
            simulatedCycles++; // Base cost for any instruction
            size_t current_ip = ip; // Save the PC of the current instruction
            
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

                case OP_EQUAL: {
                    int b = pop();
                    int a = pop();
                    push(a == b);
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
                    std::cout << "\nExecution finished in " << simulatedCycles << " CPU cycles.\n";
                    predictor.printStats();
                    return; 
  
                case OP_JUMP: {
                    ip = bytecode[ip];
                    break;
                }

                case OP_JUMP_IF_FALSE: {
                    int cond = pop();
                    uint8_t target = bytecode[ip++]; 
                    
                    // 1. Predict
                    size_t predictedTarget = 0;
                    bool predictedTaken = predictor.predict(current_ip, predictedTarget);
                    
                    // 2. Evaluate actual condition
                    bool actuallyTaken = (cond == 0); 

                    // 3. Simulate Hardware Penalties
                    if (predictedTaken != actuallyTaken) {
                        // Pipeline Flush Penalty
                        simulatedCycles += 5; 
                    }

                    // 4. Execute jump
                    if (actuallyTaken) ip = target;

                    // 5. Train predictor
                    predictor.update(current_ip, target, actuallyTaken, predictedTaken);
                    break;
                }

                default:
                    std::cerr << "Runtime Error: Unknown opcode " << (int)instruction << std::endl;
                    return;
            }
        }
    }
};
