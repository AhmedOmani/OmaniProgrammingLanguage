#pragma once
#include "Tokenization.hpp"

struct NodeExpr {
    Token int_lit;
};

struct NodeExit {
    NodeExpr expr ;
};
class Parser {
public:
    //Warning for naming here if something went wrong!
    inline explicit Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {

    }

    std::optional<NodeExpr> parse_expr() {
        if (peak().has_value() && peak().value().type == TokenType::int_lit) {
            return NodeExpr{.int_lit = consume()} ;
        } else {
            return {} ;
        }
    }

    std::optional<NodeExit> parse() {
        std::optional<NodeExit> exit_node ;

        while (peak().has_value()) {
            if (peak().value().type == TokenType::exit) {
                consume() ;
                if (auto node_expr = parse_expr()) {
                    exit_node = NodeExit{.expr = node_expr.value()} ;
                }
                else {
                    std::cerr << "Invalid expression" << std::endl;
                    exit(EXIT_FAILURE) ;
                }

                if (!peak().has_value() || peak().value().type != TokenType::semi) {
                    std::cerr << "Invalid expression" << std::endl;
                    exit(EXIT_FAILURE) ;
                } else {
                    consume();
                }
            }
        }
        cur_idx = 0 ;
        return exit_node ;
    }
private:
    const std::vector<Token> tokens ;

    [[nodiscard]] inline std::optional<Token> peak(int jump = 1) {
        if (cur_idx + jump > tokens.size())
            return {} ;
        else
            return tokens.at(cur_idx) ;
    }

    inline Token consume() {
        return tokens.at(cur_idx++);
    }

    size_t cur_idx = 0 ;

};