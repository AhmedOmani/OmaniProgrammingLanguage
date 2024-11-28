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
    semi
};

struct Token {
    TokenType type ;
    std::optional<std::string> value;
};

class Tokenizer  {

public:

    inline explicit Tokenizer(std::string content) : file_content(std::move(content)) {}

    inline std::vector<Token> tokenize() {

        std::string cur_str = "" ;
        std::vector<Token> tokens ;

        while (peak().has_value()) {
            //std::cout << cur_idx << std::endl;
            if (std::isalpha(peak().value())) {
                cur_str.push_back(consume()) ;

                while (peak().has_value() && std::isalnum(peak().value()))
                    cur_str.push_back(consume()) ;

                if (cur_str == "exit") {
                    tokens.push_back({.type = TokenType::exit}) ;
                    cur_str.clear() ;
                } else {
                    std::cerr << "Enta K7yan yasta! Ekteb Syntax s7 pls!\n" ;
                    exit(EXIT_FAILURE) ;
                }
            }

            else if (std::isdigit(peak().value())) {
                cur_str.push_back(consume()) ;

                while (peak().has_value() && std::isdigit(peak().value()))
                    cur_str.push_back(consume()) ;

                tokens.push_back({.type = TokenType::int_lit , .value = cur_str}) ;
                cur_str = "" ;
                continue;
            }

            else if (peak().value() == ';') {
                tokens.push_back({.type = TokenType::semi});
                cur_str.push_back(consume()) ;
                continue;
            }

            else if (std::isspace(peak().value())) {
                consume();
                continue ;
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
    [[nodiscard]] inline std::optional<char> peak(int jump = 1) const {
       if (cur_idx + jump > file_content.length()) {
            return {};
       } else {
            return file_content.at(cur_idx) ;
       }
    }

    inline char consume() {
        return file_content.at(cur_idx++) ;
    }
    std::string file_content;
    int cur_idx = 0 ;
};