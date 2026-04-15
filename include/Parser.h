#pragma once
#include "Common.h"

// --- AST ---
struct ASTNode { virtual ~ASTNode() = default; };
struct ExprNode : ASTNode {};

struct IntNode : ExprNode { int value; IntNode(int v) : value(v) {} };
struct VarNode : ExprNode { std::string name; VarNode(std::string n) : name(n) {} };

struct AddNode : ExprNode {
    std::unique_ptr<ExprNode> l, r;
    AddNode(std::unique_ptr<ExprNode> a, std::unique_ptr<ExprNode> b)
        : l(std::move(a)), r(std::move(b)) {}
};

struct SubNode : ExprNode {
    std::unique_ptr<ExprNode> l, r;
    SubNode(std::unique_ptr<ExprNode> a, std::unique_ptr<ExprNode> b)
        : l(std::move(a)), r(std::move(b)) {}
};

struct MulNode : ExprNode {
    std::unique_ptr<ExprNode> l, r;
    MulNode(std::unique_ptr<ExprNode> a, std::unique_ptr<ExprNode> b)
        : l(std::move(a)), r(std::move(b)) {}
};

struct DivNode : ExprNode {
    std::unique_ptr<ExprNode> l, r;
    DivNode(std::unique_ptr<ExprNode> a, std::unique_ptr<ExprNode> b)
        : l(std::move(a)), r(std::move(b)) {}
};

// --- STATEMENTS ---
struct StmtNode : ASTNode {};

struct LetNode : StmtNode {
    std::string name;
    std::unique_ptr<ExprNode> expr;
    LetNode(std::string n, std::unique_ptr<ExprNode> e)
        : name(n), expr(std::move(e)) {}
};

struct PrintNode : StmtNode {
    std::unique_ptr<ExprNode> expr;
    PrintNode(std::unique_ptr<ExprNode> e)
        : expr(std::move(e)) {}
};

struct BlockNode : StmtNode {
    std::vector<std::unique_ptr<StmtNode>> stmts;
};

struct IfNode : StmtNode {
    std::unique_ptr<ExprNode> cond;
    std::unique_ptr<BlockNode> thenBlock;
    std::unique_ptr<BlockNode> elseBlock;
};

// --- PARSER ---
class Parser {
    std::vector<Token> tokens;
    size_t current = 0;

    Token advance() { return tokens[current++]; }
    Token peek() { return tokens[current]; }

    bool match(TokenType t) {
        if (peek().type == t) { advance(); return true; }
        return false;
    }

    std::unique_ptr<ExprNode> primary() {
        if (match(TOK_INT))
            return std::make_unique<IntNode>(std::stoi(tokens[current-1].lexeme));

        if (match(TOK_IDENT))
            return std::make_unique<VarNode>(tokens[current-1].lexeme);

        if (match(TOK_LPAREN)) {
            auto e = expression();
            match(TOK_RPAREN);
            return e;
        }

        return nullptr;
    }

    std::unique_ptr<ExprNode> term() {
        auto left = primary();
        while (true) {
            if (match(TOK_STAR))
                left = std::make_unique<MulNode>(std::move(left), primary());
            else if (match(TOK_SLASH))
                left = std::make_unique<DivNode>(std::move(left), primary());
            else break;
        }
        return left;
    }

    std::unique_ptr<ExprNode> expression() {
        auto left = term();
        while (true) {
            if (match(TOK_PLUS))
                left = std::make_unique<AddNode>(std::move(left), term());
            else if (match(TOK_MINUS))
                left = std::make_unique<SubNode>(std::move(left), term());
            else break;
        }
        return left;
    }

    std::unique_ptr<BlockNode> block() {
        auto b = std::make_unique<BlockNode>();
        match(TOK_LBRACE);
        while (!match(TOK_RBRACE)) {
            b->stmts.push_back(statement());
        }
        return b;
    }

    std::unique_ptr<StmtNode> statement() {

        if (match(TOK_LET)) {
            std::string name = advance().lexeme;
            match(TOK_ASSIGN);
            auto e = expression();
            match(TOK_SEMI);
            return std::make_unique<LetNode>(name, std::move(e));
        }

        if (match(TOK_PRINT)) {
            auto e = expression();
            match(TOK_SEMI);
            return std::make_unique<PrintNode>(std::move(e));
        }

        if (match(TOK_IF)) {
            match(TOK_LPAREN);
            auto cond = expression();
            match(TOK_RPAREN);

            auto thenBlock = block();

            std::unique_ptr<BlockNode> elseBlock = nullptr;
            if (match(TOK_ELSE)) {
                elseBlock = block();
            }

            auto node = std::make_unique<IfNode>();
            node->cond = std::move(cond);
            node->thenBlock = std::move(thenBlock);
            node->elseBlock = std::move(elseBlock);
            return node;
        }

        return nullptr;
    }

public:
    std::vector<std::unique_ptr<StmtNode>> parse(std::vector<Token> t) {
        tokens = t;
        current = 0;

        std::vector<std::unique_ptr<StmtNode>> out;
        while (peek().type != TOK_EOF) {
            out.push_back(statement());
        }
        return out;
    }
};
