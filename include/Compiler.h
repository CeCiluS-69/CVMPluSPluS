#pragma once
#include "Common.h"
#include "Parser.h"

class Compiler {
    std::vector<uint8_t> bytecode;
    std::unordered_map<std::string, uint8_t> varMap;
    uint8_t varCounter = 0;

    void emit(uint8_t byte) { bytecode.push_back(byte); }

    void compileExpr(ExprNode* expr) {

        // --- INTEGER ---
        if (auto* intNode = dynamic_cast<IntNode*>(expr)) {
            emit(OP_PUSH);
            emit((uint8_t)intNode->value);
        }

        // --- VARIABLE ---
        else if (auto* varNode = dynamic_cast<VarNode*>(expr)) {
            emit(OP_LOAD);
            emit(varMap[varNode->name]);
        }

        // --- ADD ---
        else if (auto* addNode = dynamic_cast<AddNode*>(expr)) {
            compileExpr(addNode->left.get());
            compileExpr(addNode->right.get());
            emit(OP_ADD);
        }

        // --- SUB ---
        else if (auto* subNode = dynamic_cast<SubNode*>(expr)) {
            compileExpr(subNode->left.get());
            compileExpr(subNode->right.get());
            emit(OP_SUB);
        }

        // --- MUL ---
        else if (auto* mulNode = dynamic_cast<MulNode*>(expr)) {
            compileExpr(mulNode->left.get());
            compileExpr(mulNode->right.get());
            emit(OP_MUL);
        }

        // --- DIV ---
        else if (auto* divNode = dynamic_cast<DivNode*>(expr)) {
            compileExpr(divNode->left.get());
            compileExpr(divNode->right.get());
            emit(OP_DIV);
        }
    }

public:
    std::vector<uint8_t> compile(const std::vector<std::unique_ptr<StmtNode>>& ast) {
        for (const auto& stmt : ast) {

            // --- LET STATEMENT ---
            if (auto* letNode = dynamic_cast<LetNode*>(stmt.get())) {
                compileExpr(letNode->expr.get());

                if (varMap.find(letNode->varName) == varMap.end()) {
                    varMap[letNode->varName] = varCounter++;
                }

                emit(OP_STORE);
                emit(varMap[letNode->varName]);
            }

            // --- PRINT STATEMENT ---
            else if (auto* printNode = dynamic_cast<PrintNode*>(stmt.get())) {
                compileExpr(printNode->expr.get());
                emit(OP_PRINT);
            }
        }

        emit(OP_HALT);
        return bytecode;
    }
};
