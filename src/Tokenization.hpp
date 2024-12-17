#pragma once
#include<bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>

enum class TokenType {
    exit,
    int_lit,
    semi,
    open_paren ,
    close_paren,
    ident,
    omani,
    eq ,
    plus ,
    mult ,
    sub ,
    div ,
    open_curly,
    close_curly,
    if_,
    elif,
    else_,
};

std::optional<int> bin_prec(TokenType type) {
    switch (type) {
        case TokenType::plus:
            return 0 ;
        case TokenType::mult:
            return 1 ;
        case TokenType::sub:
            return 0 ;
        case TokenType::div:
            return 1 ;
        default:
            return {} ;
    }
}
struct Token {
    TokenType type;
    std::optional<std::string> value;

    std::string toString() const {
        std::string typeStr = tokenTypeToString(type);
        if (value.has_value()) {
            return typeStr + "('" + value.value() + "')";
        }
        return typeStr;
    }

    private:
    static std::string tokenTypeToString(TokenType type) {
        switch (type) {
            case TokenType::exit: return "exit";
            case TokenType::int_lit: return "int_lit";
            case TokenType::semi: return "semi";
            case TokenType::open_paren: return "open_paren";
            case TokenType::close_paren: return "close_paren";
            case TokenType::ident: return "ident";
            case TokenType::omani: return "omani";
            case TokenType::eq: return "eq";
            case TokenType::plus: return "plus";
            case TokenType::mult: return "mult";
            case TokenType::open_curly: return "{" ;
            case TokenType::close_curly: return "}" ;
            case TokenType::if_: return "if" ;
            default: return "unknown";
        }
    }
};

class Tokenizer  {

public:

    inline explicit Tokenizer(std::string content) : file_content(std::move(content)) {}

    inline std::vector<Token> tokenize() {

        std::string cur_str = "" ;
        std::vector<Token> tokens ;

        while (peak().has_value()) {

            if (std::isalpha(peak().value())) {
                cur_str.push_back(consume()) ;

                while (peak().has_value() && std::isalnum(peak().value()))
                    cur_str.push_back(consume()) ;

                if (cur_str == "exit") {
                    tokens.push_back({.type = TokenType::exit}) ;
                } else if (cur_str == "omani") {
                    tokens.push_back({.type = TokenType::omani}) ;
                } else if (cur_str == "if") {
                    tokens.push_back({.type = TokenType::if_}) ;
                } else if (cur_str == "elif") {
                    tokens.push_back({.type = TokenType::elif}) ;
                } else if (cur_str == "else") {
                    tokens.push_back({.type = TokenType::else_}) ;
                }
                else {
                    tokens.push_back({.type = TokenType::ident , .value = cur_str}) ;
                }
                cur_str.clear() ;
            }

            else if (std::isdigit(peak().value())) {
                cur_str.push_back(consume()) ;

                while (peak().has_value() && std::isdigit(peak().value()))
                    cur_str.push_back(consume()) ;

                tokens.push_back({.type = TokenType::int_lit , .value = cur_str}) ;
                cur_str = "" ;
                continue;
            }

            else if (peak().value() == '/' && peak(1).has_value() && peak(1).value() == '/') {
                consume() ;
                consume() ;
                while (peak().has_value() && peak().value() != '\n') consume() ;
            }

            else if (peak().value() == '/' && peak(1).has_value() && peak(1).value() == '*') {
                consume() ;
                consume() ;
                while (peak().has_value()) {
                    if (peak().value() == '*' && peak(1).has_value() && peak(1).value() == '/') {
                        break;
                    }
                    std::cout << consume() << std::endl;
                }
                if (peak().has_value())
                    std::cout << consume() << std::endl ;
                if (peak().has_value())
                    consume() ;
            }
            else if (peak().value() == ';') {
                tokens.push_back({.type = TokenType::semi});
                consume() ;
            }

            else if (peak().value() == '=') {
                tokens.push_back({.type = TokenType::eq});
                consume() ;
            }

            else if (std::isspace(peak().value())) {
                consume();
            }

            else if (peak().value() == '(') {
                tokens.push_back({.type = TokenType::open_paren}) ;
                consume() ;
            }

            else if (peak().value() == ')') {
                tokens.push_back({.type = TokenType::close_paren}) ;
                consume() ;
            }

            else if (peak().value() == '+') {
                tokens.push_back({.type = TokenType::plus}) ;
                consume() ;
            }

            else if (peak().value() == '*') {
                tokens.push_back({.type = TokenType::mult}) ;
                consume() ;
            }

            else if (peak().value() == '/') {
                tokens.push_back({.type = TokenType::div}) ;
                consume() ;
            }

            else if (peak().value() == '-') {
                tokens.push_back({.type = TokenType::sub}) ;
                consume() ;
            }

            else if (peak().value() == '{') {
                tokens.push_back({.type = TokenType::open_curly}) ;
                consume() ;
            }

            else if (peak().value() == '}') {
                tokens.push_back({.type = TokenType::close_curly}) ;
                consume() ;
            }

            else {
                std::cerr << "Enta K7yan yasta! Ekteb Syntax s7 pls!\n" ;
                exit(EXIT_FAILURE) ;
            }

        }
        cur_idx = 0 ;
        return tokens ;
    }

private:
    [[nodiscard]] inline std::optional<char> peak(int jump = 0) const {
       if (cur_idx + jump >= file_content.length()) {
            return {};
       } else {
            return file_content.at(cur_idx + jump) ;
       }
    }

    inline char consume() {
        return file_content.at(cur_idx++) ;
    }
    std::string file_content;
    int cur_idx = 0 ;
};