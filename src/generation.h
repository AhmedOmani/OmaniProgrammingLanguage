#pragma once
#include "parser.h"

class Generator {
public:
    inline Generator(NodeExit root) : root(std::move(root)) {

    }

    [[nodiscard]]std::string generate() const {
        std::stringstream output ;
        output << "global _start\n_start:\n";
        output << "    mov rax, 60\n" ;
        output << "    mov rdi, " << root.expr.int_lit.value.value() << "\n" ;
        output << "    syscall";
        return output.str() ;
    }
private:
    const NodeExit root ;
};