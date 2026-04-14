#pragma once
#include "Common.h"
#include "Parser.h"

class Compiler {
    std::vector<uint8_t> bytecode;
    std::unordered_map<std::string, uint8_t> varMap;
    uint8_t varCounter = 0;

    void emit(uint8_t byte) { bytecode.push_back(byte); }

    void compileExpr(ExprNode* expr) {
        if (auto* intNode = dynamic_cast<IntNode*>(expr)) {
            emit(OP_PUSH);
            emit((uint8_t)intNode->value);
        } 
        else if (auto* varNode = dynamic_cast<VarNode*>(expr)) {
            emit(OP_LOAD);
            emit(varMap[varNode->name]);
        }
        else if (auto* addNode = dynamic_cast<AddNode*>(expr)) {
            compileExpr(addNode->left.get());
            compileExpr(addNode->right.get());
            emit(OP_ADD);
        }
    }

public:
    std::vector<uint8_t> compile(const std::vector<std::unique_ptr<StmtNode>>& ast) {
        for (const auto& stmt : ast) {
            if (auto* letNode = dynamic_cast<LetNode*>(stmt.get())) {
                compileExpr(letNode->expr.get());
                if (varMap.find(letNode->varName) == varMap.end()) {
                    varMap[letNode->varName] = varCounter++;
                }
                emit(OP_STORE);
                emit(varMap[letNode->varName]);
            } 
            else if (auto* printNode = dynamic_cast<PrintNode*>(stmt.get())) {
                compileExpr(printNode->expr.get());
                emit(OP_PRINT);
            }
        }
        emit(OP_HALT);
        return bytecode;
    }
};
