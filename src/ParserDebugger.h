#pragma once
#include<bits/stdc++.h>
#include "parser.h"
#include "Tokenization.hpp"

// DebugPrinter Class
class DebugPrinter {
public:
     std::string print(std::optional<NodeProg> prog) {
        std::ostringstream oss;
        oss << "Program:\n";
        for (NodeStmt* stmt : prog.value().stmts) {
            oss << print(stmt) << "\n";
        }
        return oss.str();
    }

private:
    static std::string print(NodeStmt* stmt) {
        if (auto exitStmt = std::get_if<NodeStmtExit*>(&stmt->var)) {
            return "exit -> " + print((*exitStmt)->expr);
        } else if (auto omaniStmt = std::get_if<NodeStmtOmani*>(&stmt->var)) {
            return "omani -> " + (*omaniStmt)->ident.toString() + " = " + print((*omaniStmt)->expr);
        }
        return "Unknown Stmt";
    }

    static std::string print(NodeExpr* expr) {
        if (auto termExpr = std::get_if<NodeTermExpr*>(&expr->var)) {
            return "ExprTerm -> " + print(*termExpr);
        } else if (auto binExpr = std::get_if<NodeBinExpr*>(&expr->var)) {
            return "ExprBin -> " + print(*binExpr);
        }
        return "Unknown Expr";
    }

    static std::string print(NodeTermExpr* termExpr) {
        if (auto intLit = std::get_if<NodeTermIntLit*>(&termExpr->var)) {
            return "IntLit(" + (*intLit)->int_lit.toString() + ")";
        } else if (auto iden = std::get_if<NodeTermIden*>(&termExpr->var)) {
            return "Identifier(" + (*iden)->ident.toString() + ")";
        }
        return "Unknown TermExpr";
    }

    static std::string print(NodeBinExpr* binExpr) {
        if (auto addExpr = binExpr->add) {
            return "Add(" + print(addExpr->lhs) + ", " + print(addExpr->rhs) + ")";
        }
        return "Unknown BinExpr";
    }
};