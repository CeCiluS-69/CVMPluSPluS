#pragma once
#include "Common.h"

// --- AST NODES ---
struct ASTNode { virtual ~ASTNode() = default; };
struct ExprNode : ASTNode {};
struct IntNode : ExprNode { int value; IntNode(int v) : value(v) {} };
struct VarNode : ExprNode { std::string name; VarNode(std::string n) : name(n) {} };
struct AddNode : ExprNode { 
    std::unique_ptr<ExprNode> left, right; 
    AddNode(std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r) : left(std::move(l)), right(std::move(r)) {}
};

struct StmtNode : ASTNode {};
struct LetNode : StmtNode { 
    std::string varName; std::unique_ptr<ExprNode> expr; 
    LetNode(std::string n, std::unique_ptr<ExprNode> e) : varName(n), expr(std::move(e)) {}
};
struct PrintNode : StmtNode { 
    std::unique_ptr<ExprNode> expr; 
    PrintNode(std::unique_ptr<ExprNode> e) : expr(std::move(e)) {}
};

// --- PARSER ---
class Parser {
    std::vector<Token> tokens;
    size_t current = 0;

    Token advance() { return tokens[current++]; }
    Token peek() { return tokens[current]; }
    bool match(TokenType type) {
        if (peek().type == type) { advance(); return true; }
        return false;
    }

    std::unique_ptr<ExprNode> parsePrimary() {
        if (match(TOK_INT)) return std::make_unique<IntNode>(std::stoi(tokens[current-1].lexeme));
        if (match(TOK_IDENT)) return std::make_unique<VarNode>(tokens[current-1].lexeme);
        return nullptr;
    }

    std::unique_ptr<ExprNode> parseExpression() {
        auto left = parsePrimary();
        while (match(TOK_PLUS)) {
            auto right = parsePrimary();
            left = std::make_unique<AddNode>(std::move(left), std::move(right));
        }
        return left;
    }

public:
    std::vector<std::unique_ptr<StmtNode>> parse(std::vector<Token> t) {
        tokens = t;
        std::vector<std::unique_ptr<StmtNode>> statements;

        while (peek().type != TOK_EOF) {
            if (match(TOK_LET)) {
                std::string name = advance().lexeme;
                match(TOK_ASSIGN);
                auto expr = parseExpression();
                match(TOK_SEMI);
                statements.push_back(std::make_unique<LetNode>(name, std::move(expr)));
            } else if (match(TOK_PRINT)) {
                auto expr = parseExpression();
                match(TOK_SEMI);
                statements.push_back(std::make_unique<PrintNode>(std::move(expr)));
            }
        }
        return statements;
    }
};
