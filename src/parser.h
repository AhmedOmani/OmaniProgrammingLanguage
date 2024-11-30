#pragma once
#include "Tokenization.hpp"
#include <variant>

using namespace std ;
struct NodeExprIntLit {
    Token int_lit ;
};

struct NodeExprIdent {
    Token ident ;
};

struct NodeExpr {
    std::variant<NodeExprIntLit , NodeExprIdent> var ;
};

struct NodeStmtExit {
    NodeExpr expr ;
};

struct NodeStmtOmani {
    Token ident ;
    NodeExpr expr ;
};
struct NodeStmt {
    std::variant<NodeStmtExit , NodeStmtOmani> var ;
};

struct NodeProg {
    std::vector<NodeStmt> stmts ;
};

class Parser {
public:
    //Warning for naming here if something went wrong!
    inline explicit Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

    std::optional<NodeExpr> parse_expr() {
        if (peak().has_value() && peak().value().type == TokenType::int_lit)
            return NodeExpr{.var = NodeExprIntLit{.int_lit = consume() } } ;

        else if (peak().has_value() && peak().value().type == TokenType::ident)
            return NodeExpr{.var = NodeExprIdent{.ident = consume() }} ;

        else
            return {} ;

    }

    std::optional<NodeStmt> parse_stmt() {

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
            NodeStmtExit stmt_exit ;
            if (auto node_expr = parse_expr()) {
                stmt_exit = {.expr = node_expr.value()} ;
            }
            else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE) ;
            }

            if (peak().has_value() && peak().value().type == TokenType::close_paren) {
                consume() ;
            } else {
                std::cerr << "Expected ')'" << std::endl;
                exit(EXIT_FAILURE) ;
            }

            if (peak().has_value() && peak().value().type == TokenType::semi) {
                consume();
            } else {
                std::cerr << "Expected ';'" << std::endl;
                exit(EXIT_FAILURE) ;
            }

            return NodeStmt{.var = stmt_exit} ;
        }

        else if (validateOmaniSyntax()) {
            consume() ;
            auto stmt_omani = NodeStmtOmani{.ident = consume()} ;
            consume() ;
            if (auto expr = parse_expr()) {
                stmt_omani.expr = expr.value() ;
            } else {
                std::cerr << "Invalid expression for assign operator" << std::endl;
                exit(EXIT_FAILURE) ;
            }

            if (peak().has_value() && peak().value().type == TokenType::semi) {
                consume() ;
            } else {
                std::cerr << "Mtensash el ';' yasta" << std::endl;
                exit(EXIT_FAILURE) ;
            }

            return NodeStmt{.var = stmt_omani};

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

    size_t cur_idx = 0 ;

};