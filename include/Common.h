#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <unordered_map>

// --- SHARED TOKENS ---
enum TokenType { 
    TOK_LET, TOK_PRINT, TOK_IDENT, TOK_INT, 
    TOK_PLUS, TOK_ASSIGN, TOK_SEMI, TOK_EOF 
};

struct Token {
    TokenType type;
    std::string lexeme;
};

// --- SHARED OPCODES ---
enum Opcode : uint8_t {
    OP_PUSH,    // Push integer onto stack
    OP_ADD,     // Pop 2, Add, Push result
    OP_STORE,   // Pop value, store in variable slot
    OP_LOAD,    // Load from variable slot, push to stack
    OP_PRINT,   // Pop value and print
    OP_HALT     // End execution
};
