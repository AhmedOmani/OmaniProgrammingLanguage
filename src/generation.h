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
                if (!gen->variables.contains(term_iden->ident.value.value())) {
                    std::cerr << "Elvariable da m4 mt3raf " << "(" << term_iden->ident.value.value() << ")\n";
                    exit(EXIT_FAILURE) ;
                }
                const auto& var = gen->variables.at(term_iden->ident.value.value()) ;
                std::stringstream offset ;
                offset << "QWORD [rsp + " << (gen->stack_size - var.stack_loc - 1) * 8 << "]";
                gen->push(offset.str()) ;
            }
        };
        TermVisitor term_visitor {.gen = this} ;
        std::visit(term_visitor , term->var) ;
    }

    void gen_expr(const NodeExpr* expr)  {

        struct ExprVisitor {
            Generator *gen ;

            void operator() (const NodeTermExpr* term_expr) const {
                gen->gen_term(term_expr) ;
            }

            void operator() (const NodeBinExpr* bin_expr) const {
                gen->gen_expr(bin_expr->add->lhs) ;
                gen->gen_expr(bin_expr->add->rhs) ;
                gen->pop("rax") ;
                gen->pop("rbx") ;
                gen->output << "    add rax, rbx\n" ;
                gen->push("rax") ;
            }
        };
        ExprVisitor visitor({.gen = this}) ;
        std::visit(visitor , expr->var) ;
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
                if (gen->variables.contains(stmt_omani->ident.value.value())) {
                    std::cerr << "Yasta enta meta5dem nfs elvariable akter mn mara rekeez ;( " << stmt_omani->ident.value.value() << std::endl ;
                    exit(EXIT_FAILURE) ;
                }
                gen->variables.insert({stmt_omani->ident.value.value() , Var {.stack_loc = gen->stack_size } });
                gen->gen_expr(stmt_omani->expr) ;

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
    struct Var {
        size_t stack_loc ;
    };
    const NodeProg root ;
    std::stringstream output ;
    size_t stack_size = 0 ;
    std::map<std::string , Var> variables ;
};