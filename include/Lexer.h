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

            if (c == '+') { tokens.push_back({TOK_PLUS, "+"}); i++; continue; }
            if (c == '-') { tokens.push_back({TOK_MINUS, "-"}); i++; continue; }
            if (c == '*') { tokens.push_back({TOK_STAR, "*"}); i++; continue; }
            if (c == '/') { tokens.push_back({TOK_SLASH, "/"}); i++; continue; }

            if (c == '<') { tokens.push_back({TOK_LESS, "<"}); i++; continue; }   
            if (c == '>') { tokens.push_back({TOK_GREATER, ">"}); i++; continue; }

            if (c == '=') { tokens.push_back({TOK_ASSIGN, "="}); i++; continue; }
            if (c == ';') { tokens.push_back({TOK_SEMI, ";"}); i++; continue; }

            if (c == '(') { tokens.push_back({TOK_LPAREN, "("}); i++; continue; }
            if (c == ')') { tokens.push_back({TOK_RPAREN, ")"}); i++; continue; }

            if (c == '{') { tokens.push_back({TOK_LBRACE, "{"}); i++; continue; }
            if (c == '}') { tokens.push_back({TOK_RBRACE, "}"}); i++; continue; }

            if (std::isdigit(c)) {
                std::string num;
                while (i < source.length() && std::isdigit(source[i])) {
                    num += source[i++];
                }
                tokens.push_back({TOK_INT, num});
                continue;
            }

            if (std::isalpha(c)) {
                std::string ident;
                while (i < source.length() && std::isalnum(source[i])) {
                    ident += source[i++];
                }

                if (ident == "let") tokens.push_back({TOK_LET, ident});
                else if (ident == "print") tokens.push_back({TOK_PRINT, ident});
                else if (ident == "if") tokens.push_back({TOK_IF, ident});
                else if (ident == "else") tokens.push_back({TOK_ELSE, ident});
                else if (ident == "while") tokens.push_back({TOK_WHILE, ident});
                else if (ident == "for") tokens.push_back({TOK_FOR, ident}); // [NEW]
                else if (ident == "do") tokens.push_back({TOK_DO, ident});   // [NEW]
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
