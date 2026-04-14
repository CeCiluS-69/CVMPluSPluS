#pragma once
#include "Common.h"

// --- AST NODES ---
struct ASTNode { virtual ~ASTNode() = default; };
struct ExprNode : ASTNode {};

struct IntNode : ExprNode {
    int value;
    IntNode(int v) : value(v) {}
};

struct VarNode : ExprNode {
    std::string name;
    VarNode(std::string n) : name(n) {}
};

struct AddNode : ExprNode {
    std::unique_ptr<ExprNode> left, right;
    AddNode(std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r)
        : left(std::move(l)), right(std::move(r)) {}
};

struct SubNode : ExprNode {
    std::unique_ptr<ExprNode> left, right;
    SubNode(std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r)
        : left(std::move(l)), right(std::move(r)) {}
};

struct MulNode : ExprNode {
    std::unique_ptr<ExprNode> left, right;
    MulNode(std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r)
        : left(std::move(l)), right(std::move(r)) {}
};

struct DivNode : ExprNode {
    std::unique_ptr<ExprNode> left, right;
    DivNode(std::unique_ptr<ExprNode> l, std::unique_ptr<ExprNode> r)
        : left(std::move(l)), right(std::move(r)) {}
};

// --- STATEMENTS ---
struct StmtNode : ASTNode {};

struct LetNode : StmtNode {
    std::string varName;
    std::unique_ptr<ExprNode> expr;
    LetNode(std::string n, std::unique_ptr<ExprNode> e)
        : varName(n), expr(std::move(e)) {}
};

struct PrintNode : StmtNode {
    std::unique_ptr<ExprNode> expr;
    PrintNode(std::unique_ptr<ExprNode> e)
        : expr(std::move(e)) {}
};

// --- PARSER ---
class Parser {
    std::vector<Token> tokens;
    size_t current = 0;

    Token advance() { return tokens[current++]; }
    Token peek() { return tokens[current]; }

    bool match(TokenType type) {
        if (peek().type == type) {
            advance();
            return true;
        }
        return false;
    }

    // --- PRIMARY ---
    std::unique_ptr<ExprNode> parsePrimary() {
        if (match(TOK_INT))
            return std::make_unique<IntNode>(std::stoi(tokens[current - 1].lexeme));

        if (match(TOK_IDENT))
            return std::make_unique<VarNode>(tokens[current - 1].lexeme);

        if (match(TOK_LPAREN)) {
            auto expr = parseExpression();
            match(TOK_RPAREN); // TODO: add error handling
            return expr;
        }

        return nullptr; // can improve later
    }

    // --- MULTIPLICATION / DIVISION ---
    std::unique_ptr<ExprNode> parseTerm() {
        auto left = parsePrimary();

        while (true) {
            if (match(TOK_STAR)) {
                auto right = parsePrimary();
                left = std::make_unique<MulNode>(std::move(left), std::move(right));
            } else if (match(TOK_SLASH)) {
                auto right = parsePrimary();
                left = std::make_unique<DivNode>(std::move(left), std::move(right));
            } else {
                break;
            }
        }

        return left;
    }

    // --- ADDITION / SUBTRACTION ---
    std::unique_ptr<ExprNode> parseExpression() {
        auto left = parseTerm();

        while (true) {
            if (match(TOK_PLUS)) {
                auto right = parseTerm();
                left = std::make_unique<AddNode>(std::move(left), std::move(right));
            } else if (match(TOK_MINUS)) {
                auto right = parseTerm();
                left = std::make_unique<SubNode>(std::move(left), std::move(right));
            } else {
                break;
            }
        }

        return left;
    }

public:
    std::vector<std::unique_ptr<StmtNode>> parse(std::vector<Token> t) {
        tokens = t;
        current = 0;

        std::vector<std::unique_ptr<StmtNode>> statements;

        while (peek().type != TOK_EOF) {
            if (match(TOK_LET)) {
                std::string name = advance().lexeme;
                match(TOK_ASSIGN);

                auto expr = parseExpression();
                match(TOK_SEMI);

                statements.push_back(
                    std::make_unique<LetNode>(name, std::move(expr))
                );
            } 
            else if (match(TOK_PRINT)) {
                auto expr = parseExpression();
                match(TOK_SEMI);

                statements.push_back(
                    std::make_unique<PrintNode>(std::move(expr))
                );
            }
        }

        return statements;
    }
};
