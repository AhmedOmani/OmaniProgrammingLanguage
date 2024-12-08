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

struct NodeTermParen {
    NodeExpr *expr;
};


struct NodeBinExprAdd {
    NodeExpr *lhs ;
    NodeExpr *rhs ;
};

struct NodeBinExprMult {
    NodeExpr *lhs ;
    NodeExpr *rhs ;
};

struct NodeBinExprDiv{
    NodeExpr *lhs ;
    NodeExpr *rhs ;
};

struct NodeBinExprSub {
    NodeExpr *lhs ;
    NodeExpr *rhs ;
};

struct NodeBinExpr {
    std::variant<NodeBinExprAdd* , NodeBinExprMult* , NodeBinExprSub* , NodeBinExprDiv*> var ;
};

struct NodeTermExpr {
    std::variant<NodeTermIntLit* , NodeTermIden* , NodeTermParen*> var ;
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

struct NodeStmt;

struct NodeScope {
    std::vector<NodeStmt*> stmts;
};

struct NodeStmtIf {
    NodeExpr* expr ;
    NodeScope* scope ;
};

struct NodeStmt {
    std::variant<NodeStmtExit* , NodeStmtOmani* , NodeScope* , NodeStmtIf*> var ;
};

struct NodeProg {
    std::vector<NodeStmt*> stmts ;
};

class Parser {
public:
    //Warning for naming here if something went wrong!
    inline explicit Parser(std::vector<Token> tokens)
    : tokens(std::move(tokens))
    , allocator(1024 * 1024 * 20)
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
        else if (auto open_paren = try_consume(TokenType::open_paren)) {
            auto expr = parse_expr() ;
            if (!expr.has_value()) {
                cerr << "Exepcted expression after (" << endl;
                exit(EXIT_FAILURE) ;
            }
            try_consume(TokenType::close_paren , "Exepcted ) yasta b3d El expression") ;
            auto exprOpenParen = allocator.alloc<NodeTermParen>() ;
            exprOpenParen->expr = expr.value() ;
            auto node_term = allocator.alloc<NodeTermExpr>();
            node_term->var = exprOpenParen;
            return node_term ;
        }
        else {
            return {} ;
        }
    }

    //Parsing expressions by precedence climbing algorithm
    //https://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing
    std::optional<NodeExpr*> parse_expr(int lstPrec = 0) {

        std::optional<NodeTermExpr*> term_lhs = parse_term() ;
        if (!term_lhs.has_value()) {
            return {} ;
        }
        auto expr_lhs = allocator.alloc<NodeExpr>() ;
        expr_lhs->var = term_lhs.value() ;

        while (true) {
            std::optional<Token> cur_token = peak() ;
            std::optional<int> prec ;
            if (cur_token.has_value()) {
                prec = bin_prec(cur_token->type) ;
                if (!prec.has_value() || prec < lstPrec) {
                    break;
                }
                cout << "Precedence "<< prec.value() << " checking " << endl;
            } else {
                break;
            }

            Token op = consume() ;
            int nxtPrec = prec.value() + 1 ;
            auto expr_rhs = parse_expr(nxtPrec) ;

            if (!expr_rhs.has_value()) {
                cerr << "Unable to parse expression" << std::endl;
                exit(EXIT_FAILURE) ;
            }

            auto BinExpr = allocator.alloc<NodeBinExpr>() ;
            auto tempLHSExp = allocator.alloc<NodeExpr>() ;
            if (!BinExpr) {
                cerr << "Allocation error" << endl;
                exit(EXIT_FAILURE);
            }

            if (op.type == TokenType::plus) {
                auto add = allocator.alloc<NodeBinExprAdd>() ;
                tempLHSExp->var = expr_lhs->var ;
                add->lhs = tempLHSExp ;
                add->rhs = expr_rhs.value() ;
                BinExpr->var = add ;
            } else if (op.type == TokenType::mult) {
                auto mult = allocator.alloc<NodeBinExprMult>() ;
                tempLHSExp->var = expr_lhs->var ;
                mult->lhs = tempLHSExp ;
                mult->rhs = expr_rhs.value() ;
                BinExpr->var = mult ;
            } else if (op.type == TokenType::div) {
                auto div = allocator.alloc<NodeBinExprDiv>() ;
                tempLHSExp->var = expr_lhs->var ;
                div->lhs = tempLHSExp ;
                div->rhs = expr_rhs.value() ;
                BinExpr->var = div ;
            }
            else if (op.type == TokenType::sub) {
                auto sub = allocator.alloc<NodeBinExprSub>() ;
                tempLHSExp->var = expr_lhs->var ;
                sub->lhs = tempLHSExp ;
                sub->rhs = expr_rhs.value() ;
                BinExpr->var = sub ;
            }
            expr_lhs->var = BinExpr ;
        }
        return expr_lhs ;
    }

    std::optional<NodeScope*> parse_scope() {
        if (!try_consume(TokenType::open_curly).has_value()) {
            return {} ;
        }
        auto scope = allocator.alloc<NodeScope>() ;
        while (auto stmt = parse_stmt()) scope->stmts.push_back(stmt.value()) ;
        try_consume(TokenType::close_curly , "Enta nset el '}' ya m3lm");
        return scope ;
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

        auto validScopeStmt = [&] {
            return peak().has_value() && peak().value().type == TokenType::open_curly ;
        };

        auto validateIfStmt = [&] {
            return try_consume(TokenType::if_) ;
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
        }

        else if (validScopeStmt()) {
            if (auto scope = parse_scope()) {
                auto stmt = allocator.alloc<NodeStmt>() ;
                stmt->var = scope.value() ;
                return stmt ;
            } else {
                std::cerr << "Invalid Scope" << endl;
                exit(EXIT_FAILURE);
            }
        }

        else if (validateIfStmt()) {
            try_consume(TokenType::open_paren , "Expected '('");
            auto stmt_if = allocator.alloc<NodeStmtIf>();
            if (auto expr = parse_expr()) {
                stmt_if->expr = expr.value() ;
            } else {
                std::cerr << "Invalid expression" << "\n" ;
                exit(EXIT_FAILURE) ;
            }
            try_consume(TokenType::close_paren , "Expected ')'") ;
            if (auto scope = parse_scope()) {
                stmt_if->scope = scope.value() ;
            } else {
                std::cerr << "Invalid Scope" << "\n" ;
                exit(EXIT_FAILURE) ;
            }
            auto stmt = allocator.alloc<NodeStmt>() ;
            stmt->var = stmt_if ;
            return stmt ;
        }
        else {
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
        cout << prog.stmts.size() << endl;
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