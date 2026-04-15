#pragma once
#include "Common.h"
#include "Parser.h"

class Compiler {
    std::vector<uint8_t> code;
    std::unordered_map<std::string, uint8_t> vars;
    uint8_t varCount = 0;

    void emit(uint8_t b) { code.push_back(b); }

    size_t emitJump(uint8_t op) {
        emit(op);
        emit(0); // placeholder
        return code.size() - 1;
    }

    void patchJump(size_t pos) {
        code[pos] = code.size();
    }

    void expr(ExprNode* e) {
        if (auto* n = dynamic_cast<IntNode*>(e)) {
            emit(OP_PUSH); emit(n->value);
        }
        else if (auto* v = dynamic_cast<VarNode*>(e)) {
            emit(OP_LOAD); emit(vars[v->name]);
        }
        else if (auto* a = dynamic_cast<AddNode*>(e)) {
            expr(a->l.get()); expr(a->r.get()); emit(OP_ADD);
        }
        else if (auto* s = dynamic_cast<SubNode*>(e)) {
            expr(s->l.get()); expr(s->r.get()); emit(OP_SUB);
        }
        else if (auto* m = dynamic_cast<MulNode*>(e)) {
            expr(m->l.get()); expr(m->r.get()); emit(OP_MUL);
        }
        else if (auto* d = dynamic_cast<DivNode*>(e)) {
            expr(d->l.get()); expr(d->r.get()); emit(OP_DIV);
        }
    }

    void stmt(StmtNode* s) {

        if (auto* l = dynamic_cast<LetNode*>(s)) {
            expr(l->expr.get());
            if (!vars.count(l->name)) vars[l->name] = varCount++;
            emit(OP_STORE); emit(vars[l->name]);
        }

        else if (auto* p = dynamic_cast<PrintNode*>(s)) {
            expr(p->expr.get());
            emit(OP_PRINT);
        }

        else if (auto* i = dynamic_cast<IfNode*>(s)) {
            expr(i->cond.get());

            size_t jFalse = emitJump(OP_JUMP_IF_FALSE);

            for (auto& st : i->thenBlock->stmts)
                stmt(st.get());

            size_t jEnd = emitJump(OP_JUMP);

            patchJump(jFalse);

            if (i->elseBlock) {
                for (auto& st : i->elseBlock->stmts)
                    stmt(st.get());
            }

            patchJump(jEnd);
        }
    }

public:
    std::vector<uint8_t> compile(const std::vector<std::unique_ptr<StmtNode>>& ast) {
        for (auto& s : ast) stmt(s.get());
        emit(OP_HALT);
        return code;
    }
};
