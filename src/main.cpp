#include <iostream>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>
#include "generation.h"
#include "parser.h"
using namespace std ;


// Helper functions to print nodes
std::string print_token(const Token& token) {
    return token.value.value(); // Adjust based on Token definition.
}

std::string print_node_expr(const NodeExpr& expr) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, NodeExprIntLit>)
            return "IntLit(" + print_token(arg.int_lit) + ")";
        else if constexpr (std::is_same_v<T, NodeExprIdent>)
            return "Ident(" + print_token(arg.ident) + ")";
    }, expr.var);
}

std::string print_node_stmt(const NodeStmt& stmt) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, NodeStmtExit>)
            return "ExitStmt(" + print_node_expr(arg.expr) + ")";
        else if constexpr (std::is_same_v<T, NodeStmtOmani>)
            return "OmaniStmt(ident: " + print_token(arg.ident) + ", expr: " + print_node_expr(arg.expr) + ")";
    }, stmt.var);
}

std::string print_node_prog(const NodeProg& prog) {
    std::string result = "Program:\n";
    for (const auto& stmt : prog.stmts) {
        result += "  " + print_node_stmt(stmt) + "\n";
    }
    return result;
}

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
    //debugTokens(tokens) ;

    //Parsing
    Parser parser(std::move(tokens)) ;
    std::optional<NodeProg> tree = parser.parse_prog() ;
    cout << print_node_prog(tree.value())<<endl;;

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
