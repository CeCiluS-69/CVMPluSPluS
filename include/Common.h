#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <cstdint>

// --- TOKENS ---
enum TokenType {
    TOK_LET, TOK_PRINT,
    TOK_IF, TOK_ELSE,

    TOK_IDENT, TOK_INT,

    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_ASSIGN, TOK_SEMI,

    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
TOK_WHILE,
TOK_BREAK,
TOK_CONTINUE,

    TOK_EOF
};

struct Token {
    TokenType type;
    std::string lexeme;
};

// --- OPCODES ---
enum Opcode : uint8_t {
    OP_PUSH,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV,

    OP_STORE, OP_LOAD,
    OP_PRINT,

    OP_JUMP,
    OP_JUMP_IF_FALSE,

    OP_HALT
};
