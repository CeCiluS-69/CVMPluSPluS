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

            if (std::isspace(c)) { i++; continue; }

            // Optimized single-character tokenization
            if (c == '+') { tokens.push_back({TOK_PLUS, std::string_view(source.data() + i, 1)}); i++; continue; }
            if (c == '-') { tokens.push_back({TOK_MINUS, std::string_view(source.data() + i, 1)}); i++; continue; }
            if (c == '*') { tokens.push_back({TOK_STAR, std::string_view(source.data() + i, 1)}); i++; continue; }
            if (c == '/') { tokens.push_back({TOK_SLASH, std::string_view(source.data() + i, 1)}); i++; continue; }

            if (c == '<') { tokens.push_back({TOK_LESS, std::string_view(source.data() + i, 1)}); i++; continue; }
            if (c == '>') { tokens.push_back({TOK_GREATER, std::string_view(source.data() + i, 1)}); i++; continue; }

            if (c == '=') { tokens.push_back({TOK_ASSIGN, std::string_view(source.data() + i, 1)}); i++; continue; }
            if (c == ';') { tokens.push_back({TOK_SEMI, std::string_view(source.data() + i, 1)}); i++; continue; }

            if (c == '(') { tokens.push_back({TOK_LPAREN, std::string_view(source.data() + i, 1)}); i++; continue; }
            if (c == ')') { tokens.push_back({TOK_RPAREN, std::string_view(source.data() + i, 1)}); i++; continue; }

            if (c == '{') { tokens.push_back({TOK_LBRACE, std::string_view(source.data() + i, 1)}); i++; continue; }
            if (c == '}') { tokens.push_back({TOK_RBRACE, std::string_view(source.data() + i, 1)}); i++; continue; }

            // Optimized Numbers (No concatenation)
            if (std::isdigit(c)) {
                size_t start = i;
                while (i < source.length() && std::isdigit(source[i])) i++;
                tokens.push_back({TOK_INT, std::string_view(source.data() + start, i - start)});
                continue;
            }

            // Optimized Identifiers & Keywords (No concatenation)
            if (std::isalpha(c)) {
                size_t start = i;
                while (i < source.length() && std::isalnum(source[i])) i++;
                std::string_view ident(source.data() + start, i - start);

                if (ident == "let") tokens.push_back({TOK_LET, ident});
                else if (ident == "print") tokens.push_back({TOK_PRINT, ident});
                else if (ident == "if") tokens.push_back({TOK_IF, ident});
                else if (ident == "else") tokens.push_back({TOK_ELSE, ident});
                else if (ident == "while") tokens.push_back({TOK_WHILE, ident});
                else if (ident == "for") tokens.push_back({TOK_FOR, ident}); 
                else if (ident == "do") tokens.push_back({TOK_DO, ident});   
                else if (ident == "break") tokens.push_back({TOK_BREAK, ident});
                else if (ident == "continue") tokens.push_back({TOK_CONTINUE, ident});
                else tokens.push_back({TOK_IDENT, ident});

                continue;
            }

            i++;
        }

        tokens.push_back({TOK_EOF, ""});
        return tokens;
    }
};
