#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>

enum class TokenType {
    _return,
    int_lit,
    semi
};
struct Token {
    TokenType type ;
    std::optional<std::string> value;
};

std::vector<Token> Tokenization(const std::string& str) {
    int idx = 0 ;
    std::string cur_str = "" ;
    std::vector<Token> ret ;

    while (idx < str.size()) {

        if (str[idx] == ' ') {
            idx += 1 ;
            continue;
        }

        if (std::isalpha(str[idx])) {
            int jdx = idx ;
            while (jdx < str.size() && std::isalpha(str[jdx]))
                cur_str += str[jdx] , jdx += 1 ;
            idx = jdx ;
            ret.push_back({.type = TokenType::_return}) ;
            cur_str = "" ;
        }

        if (std::isdigit(str[idx])) {
            int jdx = idx ;
            while (jdx < str.size() && std::isdigit(str[jdx]))
                cur_str += str[jdx] , jdx += 1 ;
            idx = jdx ;
            ret.push_back({.type = TokenType::int_lit , .value = cur_str}) ;
            cur_str = "" ;
        }

        if (str[idx] == ';') {
            ret.push_back({.type = TokenType::semi}) ;
            cur_str = "" ;
            idx += 2 ;

        }
    }
    return ret ;
}

std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream output ;

    output << "global _start\n_start:\n";
    for (int i = 0 ; i < tokens.size() ; i++) {
        const Token token = tokens[i] ;
        if (token.type == TokenType::_return) {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit) {
                if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi) {
                    output << "    mov rax, 60\n" ;
                    output << "    mov rdi, " << tokens.at(i + 1).value.value() << "\n" ;
                    output << "    syscall";
                }
            }
        }
    }
    return output.str() ;
}

int main(int argc , char * argv[])  {
    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is... " << std::endl ;
        std::cerr <<"Omani <input.om>" << std::endl;
        return EXIT_FAILURE ;
    }
    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in) ;
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
    }
    std::vector<Token> ret = Tokenization(contents) ;
    {
        std::fstream file("out.asm" , std::ios::out) ;
        file << tokens_to_asm(ret) ;
    }

    system("nasm -felf64 out.asm") ;
    system("ld -o out out.o") ;
    return EXIT_SUCCESS ;
}
