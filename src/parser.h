#pragma once
#include "Tokenization.hpp"
#include <variant>

#include "Arena.h"

using namespace std ;

struct NodeTermIntLit {
    Token int_lit ;
};

struct NodeTermIden {
    Token ident ;
};

struct NodeExpr;

struct NodeBinExprAdd {
    NodeExpr *lhs ;
    NodeExpr *rhs ;
};

struct NodeBinExprMult {
    NodeExpr *lhs ;
    NodeExpr *rhs ;
};

struct NodeBinExpr {
    NodeBinExprAdd* add ;
};

struct NodeTermExpr {
    std::variant<NodeTermIntLit* , NodeTermIden*> var ;
};
struct NodeExpr {
    std::variant<NodeTermExpr* , NodeBinExpr*> var ;
};

struct NodeStmtExit {
    NodeExpr* expr ;
};

struct NodeStmtOmani {
    Token ident ;
    NodeExpr* expr ;
};
struct NodeStmt {
    std::variant<NodeStmtExit* , NodeStmtOmani*> var ;
};

struct NodeProg {
    std::vector<NodeStmt*> stmts ;
};

class Parser {
public:
    //Warning for naming here if something went wrong!
    inline explicit Parser(std::vector<Token> tokens)
    : tokens(std::move(tokens))
    , allocator(1024 * 1024 * 4)
    {}

    std::optional<NodeBinExpr*> parse_bin_expr() {

    }

    std::optional<NodeTermExpr*> parse_term() {

        if (auto int_lit = try_consume(TokenType::int_lit)) {
            auto node_term_int_lit = allocator.alloc<NodeTermIntLit>() ;
            node_term_int_lit->int_lit = int_lit.value();
            auto node_term = allocator.alloc<NodeTermExpr>();
            node_term->var = node_term_int_lit ;
            return node_term ;
        }

        else if (auto ident = try_consume(TokenType::ident)) {
            auto node_term_ident = allocator.alloc<NodeTermIden>() ;
            node_term_ident->ident = ident.value() ;
            auto node_term = allocator.alloc<NodeTermExpr>();
            node_term->var = node_term_ident ;
            return node_term ;
        }

        else {
            return {} ;
        }
    }

    std::optional<NodeExpr*> parse_expr() {

        if (auto term = parse_term()) {
            if (try_consume(TokenType::plus).has_value()) {
                auto bin_expr = allocator.alloc<NodeBinExpr>() ;
                auto bin_exp_add = allocator.alloc<NodeBinExprAdd>() ;
                auto lhs_expr = allocator.alloc<NodeExpr>() ;
                lhs_expr->var = term.value() ;
                bin_exp_add->lhs = lhs_expr ;

                if (auto rhs = parse_expr()) {
                    bin_exp_add->rhs = rhs.value() ;
                    bin_expr->add = bin_exp_add ;
                    auto expr = allocator.alloc<NodeExpr>() ;
                    expr->var = bin_expr ;
                    return expr ;
                } else {
                    std::cerr<< "Experession problem" << std::endl;
                }
            }
            else {
                auto expr = allocator.alloc<NodeExpr>() ;
                expr->var = term.value() ;
                return expr ;
            }
        }
        else
            return {} ;

        if (auto bin_expr = parse_bin_expr()) {
            auto node_expr = allocator.alloc<NodeExpr>() ;
            node_expr->var = bin_expr.value() ;
            return node_expr ;
        }
        else
            return {} ;

    }

    std::optional<NodeStmt*> parse_stmt() {

        auto validateExitSyntax = [&]() {
            return peak().value().type == TokenType::exit
            && peak(1).has_value()
            && peak(1).value().type == TokenType::open_paren ;
        };

        auto validateOmaniSyntax = [&] {
            return peak().has_value()  && peak().value().type == TokenType::omani
            && peak(1).has_value() && peak(1).value().type == TokenType::ident
            && peak(2).has_value() && peak(2).value().type == TokenType::eq;
        };

        if (validateExitSyntax()) {
            consume() ;
            consume() ;
            auto stmt_exit = allocator.alloc<NodeStmtExit>();
            if (auto node_expr = parse_expr()) {
                stmt_exit->expr =  node_expr.value() ;
            }
            else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE) ;
            }

            try_consume(TokenType::close_paren , "Mtensash el ')' yasta");
            try_consume(TokenType::semi , "Mtensash el ';' yasta");

            auto node_stmt = allocator.alloc<NodeStmt>() ;
            node_stmt->var = stmt_exit ;
            return node_stmt ;
        }

        else if (validateOmaniSyntax()) {
            consume() ;
            auto stmt_omani = allocator.alloc<NodeStmtOmani>() ;
            stmt_omani->ident = consume() ;
            consume() ;
            if (auto expr = parse_expr()) {
                stmt_omani->expr = expr.value() ;
            } else {
                std::cerr << "Invalid expression for assign operator" << std::endl;
                exit(EXIT_FAILURE) ;
            }

            try_consume(TokenType::semi ,  "Mtensash el ';' yasta");

            auto node_stmt = allocator.alloc<NodeStmt>() ;
            node_stmt->var = stmt_omani ;
            return node_stmt ;
        } else {
            return {} ;
        }
    }

    std:: optional<NodeProg> parse_prog() {
        NodeProg prog ;
        while (peak().has_value()) {
            if (auto stms = parse_stmt()) {
                prog.stmts.push_back(stms.value()) ;
            } else {
                std::cerr << "Invalid statment" << std::endl;
                exit(EXIT_FAILURE) ;
            }
        }
        return prog ;
    }
private:
    const std::vector<Token> tokens ;

    [[nodiscard]] inline std::optional<Token> peak(int jump = 0) {
        if (cur_idx + jump >= tokens.size())
            return {} ;
        else
            return tokens.at(cur_idx + jump) ;
    }

    inline Token consume() {
        return tokens.at(cur_idx++);
    }

    inline Token try_consume(TokenType type , const std::string& err_msg) {
        if (peak().has_value() && peak().value().type == type)
            return consume() ;

        std::cerr << err_msg << std::endl;
        exit(EXIT_FAILURE) ;
    }

    inline std::optional<Token> try_consume(TokenType type) {
        if (peak().has_value() && peak().value().type == type)
            return consume() ;
        return {} ;
    }

    size_t cur_idx = 0 ;
    ArenaAllocator allocator ;

};