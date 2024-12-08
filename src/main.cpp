#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>
#include "generation.h"
#include "ParserDebugger.h"

#include "Arena.h"

using namespace std ;


void debugTokens(vector<Token> &tokens) {
    for (int i = 0 ; i < tokens.size() ; i++) {
        if (tokens[i].type == TokenType::semi) {
            cout << ";" << endl;
        }
        if (tokens[i].type == TokenType::eq) {
            cout << "=" << endl;
        }
        if (tokens[i].type == TokenType::close_paren) {
            cout << ")" << endl;
        }
        if (tokens[i].type == TokenType::open_paren) {
            cout << "(" << endl;
        }
        if (tokens[i].type == TokenType::omani) {
            cout << "omani" << endl;
        }
        if (tokens[i].type == TokenType::exit) {
            cout << "exit" << endl;
        }
        if (tokens[i].type == TokenType::int_lit) {
            cout << tokens[i].value.value() << endl;
        }
        if (tokens[i].type == TokenType::ident) {
            cout << "ident " << tokens[i].value.value() << endl;
        }
        if (tokens[i].type == TokenType::plus) {
            cout << "plus " << " + " << endl;
        }
        if (tokens[i].type == TokenType::mult) {
            cout << "mult " << " - " << endl;
        }
        if (tokens[i].type == TokenType::sub) {
            cout << "sub " << " - " << endl;
        }
        if (tokens[i].type == TokenType::div) {
            cout << "div " << " / " << endl;
        }

    }
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

    //Tokenizing
    Tokenizer tokenizer(std::move(contents)) ;
    std::vector<Token> tokens = tokenizer.tokenize() ;
    debugTokens(tokens) ;

    //Parsing
    Parser parser(std::move(tokens)) ;
    std::optional<NodeProg> tree = parser.parse_prog() ;

    DebugPrinter debug ;
    cout << debug.print(tree) ;

    if (!tree.has_value()) {
        std::cerr << "No exit statement found!" << std::endl;
        exit(EXIT_FAILURE) ;
    }

    //Code generation
    Generator generator(tree.value()) ;
    {
        std::fstream file("out.asm" , std::ios::out) ;
        file << generator.generate() ;
    }

    //Assembler & linker
    system("nasm -felf64 out.asm") ;
    system("ld -o out out.o") ;
    return EXIT_SUCCESS ;
}
