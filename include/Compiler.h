#pragma once
#include "Common.h"
#include "Parser.h"

class Compiler {
    std::vector<uint8_t> code;
    std::unordered_map<std::string_view, uint8_t> vars;
    
    std::vector<std::vector<size_t>> breakStack;
    std::vector<std::vector<size_t>> continueStack; 
    
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

    void patchJumpTo(size_t pos, size_t target) {
        code[pos] = target;
    }

    // Refactored to accept raw pointers
    void expr(ExprNode* e) {
        if (!e) return;

        if (auto* n = dynamic_cast<IntNode*>(e)) {
            emit(OP_PUSH); emit(n->value);
        }
        else if (auto* v = dynamic_cast<VarNode*>(e)) {
            emit(OP_LOAD); emit(vars[v->name]);
        }
        else if (auto* a = dynamic_cast<AddNode*>(e)) {
            expr(a->l); expr(a->r); emit(OP_ADD);
        }
        else if (auto* s = dynamic_cast<SubNode*>(e)) {
            expr(s->l); expr(s->r); emit(OP_SUB);
        }
        else if (auto* m = dynamic_cast<MulNode*>(e)) {
            expr(m->l); expr(m->r); emit(OP_MUL);
        }
        else if (auto* d = dynamic_cast<DivNode*>(e)) {
            expr(d->l); expr(d->r); emit(OP_DIV);
        }
        else if (auto* l = dynamic_cast<LessNode*>(e)) {           
            expr(l->l); expr(l->r); emit(OP_LESS);         
        }
        else if (auto* g = dynamic_cast<GreaterNode*>(e)) {            
            expr(g->l); expr(g->r); emit(OP_GREATER);      
        }
    }

    // Refactored to accept raw pointers
    void stmt(StmtNode* s) {
        if (!s) return;

        if (auto* l = dynamic_cast<LetNode*>(s)) {
            expr(l->expr);
            if (!vars.count(l->name)) vars[l->name] = varCount++;
            emit(OP_STORE); emit(vars[l->name]);
        }

        else if (auto* p = dynamic_cast<PrintNode*>(s)) {
            expr(p->expr);
            emit(OP_PRINT);
        }

        else if (auto* i = dynamic_cast<IfNode*>(s)) {
            expr(i->cond);

            size_t jFalse = emitJump(OP_JUMP_IF_FALSE);

            for (auto* st : i->thenBlock->stmts)
                stmt(st);

            size_t jEnd = emitJump(OP_JUMP);

            patchJump(jFalse);

            if (i->elseBlock) {
                for (auto* st : i->elseBlock->stmts)
                    stmt(st);
            }

            patchJump(jEnd);
        }
        
        else if (auto* w = dynamic_cast<WhileNode*>(s)) {
            size_t loopStart = code.size();
            
            breakStack.push_back({});
            continueStack.push_back({}); 

            expr(w->cond);

            size_t jFalse = emitJump(OP_JUMP_IF_FALSE);

            for (auto* st : w->body->stmts)
                stmt(st);

            emit(OP_JUMP);
            emit(loopStart);

            patchJump(jFalse);

            for (auto pos : breakStack.back()) patchJump(pos);
            for (auto pos : continueStack.back()) patchJumpTo(pos, loopStart);

            breakStack.pop_back();
            continueStack.pop_back();
        }

        else if (auto* d = dynamic_cast<DoWhileNode*>(s)) {
            size_t loopStart = code.size();
            
            breakStack.push_back({});
            continueStack.push_back({});

            for (auto* st : d->body->stmts)
                stmt(st);

            size_t condStart = code.size();

            expr(d->cond);
            
            size_t jFalse = emitJump(OP_JUMP_IF_FALSE);
            emit(OP_JUMP);
            emit(loopStart);
            
            patchJump(jFalse);

            for (auto pos : breakStack.back()) patchJump(pos);
            for (auto pos : continueStack.back()) patchJumpTo(pos, condStart);

            breakStack.pop_back();
            continueStack.pop_back();
        }

        else if (auto* f = dynamic_cast<ForNode*>(s)) {
            breakStack.push_back({});
            continueStack.push_back({});

            if (f->init) stmt(f->init);

            size_t loopStart = code.size();
            
            size_t jFalse = -1;
            if (f->cond) {
                expr(f->cond);
                jFalse = emitJump(OP_JUMP_IF_FALSE);
            }

            for (auto* st : f->body->stmts)
                stmt(st);

            size_t incStart = code.size();
            if (f->inc) stmt(f->inc);

            emit(OP_JUMP);
            emit(loopStart);

            if (jFalse != -1) patchJump(jFalse);

            for (auto pos : breakStack.back()) patchJump(pos);
            for (auto pos : continueStack.back()) patchJumpTo(pos, incStart);

            breakStack.pop_back();
            continueStack.pop_back();
        }

        else if (dynamic_cast<BreakNode*>(s)) {
            size_t j = emitJump(OP_JUMP);
            breakStack.back().push_back(j);
        }
        else if (dynamic_cast<ContinueNode*>(s)) {
            size_t j = emitJump(OP_JUMP);
            continueStack.back().push_back(j); 
        }
    }

public:
    std::vector<uint8_t> compile(const std::vector<StmtNode*>& ast) {
        code.reserve(1024); // Optimization
        for (auto* s : ast) stmt(s);
        emit(OP_HALT);
        return code;
    }
};
