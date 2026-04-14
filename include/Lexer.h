#pragma once
#include "Common.h"
#include <cctype>

class Lexer {
public:
    std::vector<Token> tokenize(const std::string& source) {
        std::vector<Token> tokens;
        size_t i = 0;

        while (i < source.length()) {
            char c = source[i];

            // Skip whitespace
            if (std::isspace(c)) { i++; continue; }

            // Operators
            if (c == '+') { tokens.push_back({TOK_PLUS, "+"}); i++; continue; }
            if (c == '-') { tokens.push_back({TOK_MINUS, "-"}); i++; continue; }
            if (c == '*') { tokens.push_back({TOK_STAR, "*"}); i++; continue; }
            if (c == '/') { tokens.push_back({TOK_SLASH, "/"}); i++; continue; }

            // Assignment & semicolon
            if (c == '=') { tokens.push_back({TOK_ASSIGN, "="}); i++; continue; }
            if (c == ';') { tokens.push_back({TOK_SEMI, ";"}); i++; continue; }

            // Parentheses
            if (c == '(') { tokens.push_back({TOK_LPAREN, "("}); i++; continue; }
            if (c == ')') { tokens.push_back({TOK_RPAREN, ")"}); i++; continue; }

            // Numbers
            if (std::isdigit(c)) {
                std::string num;
                while (i < source.length() && std::isdigit(source[i])) {
                    num += source[i++];
                }
                tokens.push_back({TOK_INT, num});
                continue;
            }

            // Identifiers / Keywords
            if (std::isalpha(c)) {
                std::string ident;
                while (i < source.length() && std::isalnum(source[i])) {
                    ident += source[i++];
                }

                if (ident == "let") tokens.push_back({TOK_LET, ident});
                else if (ident == "print") tokens.push_back({TOK_PRINT, ident});
                else tokens.push_back({TOK_IDENT, ident});

                continue;
            }

            // Unknown character → skip (can improve later)
            i++;
        }

        tokens.push_back({TOK_EOF, ""});
        return tokens;
    }
};
