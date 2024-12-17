#pragma once
#include "parser.h"
#include<map>

class Generator {
public:
    inline Generator(NodeProg root) : root(std::move(root)) {}

    void gen_term(const NodeTermExpr* term) {
        struct TermVisitor {
            Generator *gen ;
            void operator() (const NodeTermIntLit* term_int_lit) const {
                gen->output << "    mov rax, " << term_int_lit->int_lit.value.value() << "\n" ;
                gen->push("rax") ;
            }
            void operator() (const NodeTermIden* term_iden) const {
                auto it = std::find_if(gen->variables.cbegin() ,gen->variables.cend() ,[&](const Var &var) {
                    return var.name == term_iden->ident.value.value() ;
                });
                if (it == gen->variables.cend()) {
                    std::cerr << "Elvariable da m4 mt3raf " << "(" << term_iden->ident.value.value() << ")\n";
                    exit(EXIT_FAILURE) ;
                }
                std::stringstream offset ;
                offset << "QWORD [rsp + " << (gen->stack_size - (*it).stack_loc - 1) * 8 << "]";
                gen->push(offset.str()) ;
            }
            void operator() (const NodeTermParen* paren) {
                gen->gen_expr(paren->expr) ;
            }
        };
        TermVisitor term_visitor {.gen = this} ;
        std::visit(term_visitor , term->var) ;
    }

    void gen_bin_expr(const NodeBinExpr* bin_expr) {
        struct BinExprVisitor {
            Generator *gen ;
            void operator() (const NodeBinExprAdd* add) {
                gen->gen_expr(add->rhs) ;
                gen->gen_expr(add->lhs) ;
                gen->pop("rax") ;
                gen->pop("rbx") ;
                gen->output << "    add rax, rbx\n" ;
                gen->push("rax") ;
            }
            void operator() (const NodeBinExprMult* mult) {
                gen->gen_expr(mult->rhs) ;
                gen->gen_expr(mult->lhs) ;
                gen->pop("rax") ;
                gen->pop("rbx") ;
                gen->output << "    mul rbx\n" ;
                gen->push("rax") ;
            }
            void operator() (const NodeBinExprDiv* div) {
                gen->gen_expr(div->rhs) ;
                gen->gen_expr(div->lhs) ;
                gen->pop("rax") ;
                gen->pop("rbx") ;
                gen->output << "    div rbx\n" ;
                gen->push("rax") ;
            }
            void operator() (const NodeBinExprSub* sub) {
                gen->gen_expr(sub->rhs) ;
                gen->gen_expr(sub->lhs) ;
                gen->pop("rax") ;
                gen->pop("rbx") ;
                gen->output << "    sub rax, rbx\n" ;
                gen->push("rax") ;
            }
        };
        BinExprVisitor visitor {.gen = this} ;
        std::visit(visitor , bin_expr->var) ;
    }

    void gen_expr(const NodeExpr* expr)  {

        struct ExprVisitor {
            Generator *gen ;

            void operator() (const NodeTermExpr* term_expr) const {
                gen->gen_term(term_expr) ;
            }

            void operator() (const NodeBinExpr* bin_expr) const {
                gen->gen_bin_expr(bin_expr) ;
            }
        };
        ExprVisitor visitor({.gen = this}) ;
        std::visit(visitor , expr->var) ;
    }

    void gen_scope(const NodeScope* scope) {
        begin_scope() ;
        for (const NodeStmt* stmt : scope->stmts) gen_stmt(stmt) ;
        end_scope() ;
    }

    void gen_if_pred(NodeIfPred* pred, const std::string &end_label) {
        struct PredVisitor {
            Generator *gen ;
            const std::string &end_label ;
            void operator()(const NodeIfPredElif* elif) const {
                gen->gen_expr(elif->expr)  ;
                gen->pop("rax") ;
                std::string lstLabel = gen->create_label() ;
                gen->output << "    test rax, rax\n" ;
                gen->output << "    jz " << lstLabel << "\n" ;
                gen->gen_scope(elif->scope) ;
                gen->output << "    jmp " << end_label << "\n";
                if (elif->pred.has_value()) {
                    gen->output << lstLabel << ":\n" ;
                    gen->gen_if_pred(elif->pred.value() , end_label) ;
                }
            }
            void operator()(const NodeIfPredElse* else_) const {
                gen->gen_scope(else_->scope) ;
            }
        };
        PredVisitor visitor{.gen = this , .end_label = end_label} ;
        std::visit(visitor , pred->var) ;
    }

    void gen_stmt(const NodeStmt *stmt)  {
        struct StmtVisitor {
            Generator *gen ;
            void operator() (const NodeStmtExit* stmt_exit) const {
                gen->gen_expr(stmt_exit->expr) ;
                gen->output << "    mov rax, 60\n" ;
                gen->pop("rdi") ;
                gen->output << "    syscall\n";
            }

            void operator() (const NodeStmtOmani* stmt_omani) const {
                auto it = std::find_if(gen->variables.cbegin() ,gen->variables.cend() ,[&](const Var &var) {
                    return var.name == stmt_omani->ident.value.value() ;
                });
                if (it != gen->variables.cend()) {
                    std::cerr << "Yasta enta mesta5dem nfs elvariable akter mn mara rekeez ;( " << stmt_omani->ident.value.value() << std::endl ;
                    exit(EXIT_FAILURE) ;
                }
                gen->variables.push_back({Var {.name = stmt_omani->ident.value.value() , .stack_loc = gen->stack_size } });
                gen->gen_expr(stmt_omani->expr) ;
            }

            void operator() (const NodeScope* scope) const {
                gen->gen_scope(scope) ;
            }

            void operator() (const NodeStmtIf* stmt_if) const {
                gen->gen_expr(stmt_if->expr)  ;
                gen->pop("rax") ;
                std::string lstLabel = gen->create_label() ;
                gen->output << "    test rax, rax\n" ;
                gen->output << "    jz " << lstLabel << "\n" ;
                gen->gen_scope(stmt_if->scope) ;
                gen->output << lstLabel << ":\n" ;
                if (stmt_if->pred.has_value()) {
                    std::string end_label = gen->create_label() ;
                    gen->gen_if_pred(stmt_if->pred.value() , end_label) ;
                    gen->output << end_label << ":\n";
                }
            }
        };
        StmtVisitor visitor{.gen = this} ;
        std::visit(visitor , stmt->var) ;
    }

    [[nodiscard]]std::string generate()  {

        output << "global _start\n_start:\n";

        for ( NodeStmt* stmt : root.stmts)
            gen_stmt(stmt) ;

        output << "    mov rax, 60\n" ;
        output << "    mov rdi, 0\n" ;
        output << "    syscall";
        return output.str() ;
    }

private:
    void push(std::string reg) {
        output << "    push" << ' '  << reg << "\n" ;
        stack_size += 1 ;
    }
    void pop(std::string reg) {
        output << "    pop"  << ' '  << reg << "\n" ;
        stack_size -= 1 ;
    }
    void begin_scope() {
        scopes.push_back(variables.size()) ;
    }
    void end_scope()
    {
        size_t pop_count = variables.size() - scopes.back();
        output << "    add rsp, " << pop_count * 8 << "\n";
        stack_size -= pop_count;
        for (int i = 0; i < pop_count; i++) {
            variables.pop_back();
        }
        scopes.pop_back();
    }

    std::string create_label() {
        std::stringstream ss;
        ss << "label" << label_count++;
        return ss.str() ;
    }

    struct Var {
        string name ;
        size_t stack_loc ;
    };

    const NodeProg root ;
    std::stringstream output ;
    size_t stack_size = 0 ;
    std::vector<Var> variables ;
    std::vector<size_t> scopes{} ;
    int label_count = 0 ;
};