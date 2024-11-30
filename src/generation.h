#pragma once
#include "parser.h"
#include<map>

class Generator {
public:
    inline Generator(NodeProg root) : root(std::move(root)) {}

    void gen_expr(const NodeExpr& expr)  {

        struct ExprVisitor {
            Generator *gen ;

            void operator() (const NodeExprIntLit& expr_int_lit) {
                gen->output << "    mov rax, " << expr_int_lit.int_lit.value.value() << "\n" ;
                gen->push("rax") ;
            }

            void operator() (const NodeExprIdent& expr_ident) {
                if (!gen->variables.contains(expr_ident.ident.value.value())) {
                    std::cerr << "Elvariable da m4 mt3raf " << "(" << expr_ident.ident.value.value() << ")\n";
                    exit(EXIT_FAILURE) ;
                }
                const auto&var = gen->variables.at(expr_ident.ident.value.value()) ;
                std::stringstream offset ;
                offset << "QWORD [rsp + " << (gen->stack_size - var.stack_loc - 1) * 8 << "]";
                gen->push(offset.str()) ;
            }
        };
        ExprVisitor visitor({.gen = this}) ;
        std::visit(visitor , expr.var) ;
    }

    void gen_stmt(const NodeStmt &stmt)  {
        struct StmtVisitor {
            Generator *gen ;
            void operator() (const NodeStmtExit& stmt_exit) const {
                gen->gen_expr(stmt_exit.expr) ;
                gen->output << "    mov rax, 60\n" ;
                gen->pop("rdi") ;
                gen->output << "    syscall\n";
            }

            void operator() (const NodeStmtOmani& stmt_omani) const {
                if (gen->variables.contains(stmt_omani.ident.value.value())) {
                    std::cerr << "Yasta enta meta5dem nfs elvariable akter mn mara rekeez ;( " << stmt_omani.ident.value.value() << std::endl ;
                    exit(EXIT_FAILURE) ;
                }
                gen->variables.insert({stmt_omani.ident.value.value() , Var {.stack_loc = gen->stack_size } });
                gen->gen_expr(stmt_omani.expr) ;
            }
        };
        StmtVisitor visitor{.gen = this} ;
        std::visit(visitor , stmt.var) ;
    }

    [[nodiscard]]std::string generate()  {

        output << "global _start\n_start:\n";

        for (const NodeStmt &stmt : root.stmts)
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