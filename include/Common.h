#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <cstdint>

// --- SHARED TOKENS ---
enum TokenType { 
    TOK_LET, TOK_PRINT,
    TOK_IDENT, TOK_INT,

    // Operators
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,

    // Symbols
    TOK_ASSIGN, TOK_SEMI,
    TOK_LPAREN, TOK_RPAREN,

    TOK_EOF 
};

struct Token {
    TokenType type;
    std::string lexeme;
};

// --- SHARED OPCODES ---
enum Opcode : uint8_t {
    OP_PUSH,    // Push integer onto stack

    OP_ADD,     // a + b
    OP_SUB,     // a - b
    OP_MUL,     // a * b
    OP_DIV,     // a / b

    OP_STORE,   // store variable
    OP_LOAD,    // load variable

    OP_PRINT,   // print value
    OP_HALT     // stop execution
};
