#pragma once
#include "Common.h"
#include "Parser.h"

class Compiler {
    std::vector<uint8_t> code;
    std::unordered_map<std::string, uint8_t> vars;
    
    // [NEW] Track breaks and continues separately to allow precise patching
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

    // [NEW] Helper to patch jumps to specific targets (needed for loops)
    void patchJumpTo(size_t pos, size_t target) {
        code[pos] = target;
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
        else if (auto* l = dynamic_cast<LessNode*>(e)) {           
            expr(l->l.get()); expr(l->r.get()); emit(OP_LESS);         
        }
        else if (auto* g = dynamic_cast<GreaterNode*>(e)) {            
            expr(g->l.get()); expr(g->r.get()); emit(OP_GREATER);      
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
        
        else if (auto* w = dynamic_cast<WhileNode*>(s)) {
            size_t loopStart = code.size();
            
            breakStack.push_back({});
            continueStack.push_back({}); // [NEW]

            expr(w->cond.get());

            size_t jFalse = emitJump(OP_JUMP_IF_FALSE);

            for (auto& st : w->body->stmts)
                stmt(st.get());

            emit(OP_JUMP);
            emit(loopStart);

            patchJump(jFalse);

            for (auto pos : breakStack.back()) patchJump(pos);
            // [NEW] while continues patch to start of loop
            for (auto pos : continueStack.back()) patchJumpTo(pos, loopStart);

            breakStack.pop_back();
            continueStack.pop_back();
        }

        // [NEW] Compile Do-While
        else if (auto* d = dynamic_cast<DoWhileNode*>(s)) {
            size_t loopStart = code.size();
            
            breakStack.push_back({});
            continueStack.push_back({});

            // 1. Execute Body
            for (auto& st : d->body->stmts)
                stmt(st.get());

            size_t condStart = code.size();

            // 2. Evaluate Condition
            expr(d->cond.get());
            
            // 3. Jump based on condition
            size_t jFalse = emitJump(OP_JUMP_IF_FALSE);
            emit(OP_JUMP);
            emit(loopStart);
            
            patchJump(jFalse);

            for (auto pos : breakStack.back()) patchJump(pos);
            // Continues inside do-while jump straight to condition check
            for (auto pos : continueStack.back()) patchJumpTo(pos, condStart);

            breakStack.pop_back();
            continueStack.pop_back();
        }

        // [NEW] Compile For Loop
        else if (auto* f = dynamic_cast<ForNode*>(s)) {
            breakStack.push_back({});
            continueStack.push_back({});

            // 1. Init Step
            if (f->init) stmt(f->init.get());

            size_t loopStart = code.size();
            
            // 2. Condition Step
            size_t jFalse = -1;
            if (f->cond) {
                expr(f->cond.get());
                jFalse = emitJump(OP_JUMP_IF_FALSE);
            }

            // 3. Body
            for (auto& st : f->body->stmts)
                stmt(st.get());

            // 4. Increment Step (Continues jump here)
            size_t incStart = code.size();
            if (f->inc) stmt(f->inc.get());

            // 5. Jump back to top
            emit(OP_JUMP);
            emit(loopStart);

            // 6. Patch End
            if (jFalse != -1) patchJump(jFalse);

            for (auto pos : breakStack.back()) patchJump(pos);
            // For loops need continues to hit the increment step before looping
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
            continueStack.back().push_back(j); // [NEW] uses unified continue logic
        }
    }

public:
    std::vector<uint8_t> compile(const std::vector<std::unique_ptr<StmtNode>>& ast) {
        for (auto& s : ast) stmt(s.get());
        emit(OP_HALT);
        return code;
    }
};
