#ifndef VISITOR_H
#define VISITOR_H

#include "ast.h"
#include <string>

class ASTVisitor {
    public:
        virtual void visitNumberExpr(double val) = 0;
        virtual void visitVariableExpr(std::string name) = 0;
        virtual void visitBinaryExpr(char op, ExprAST *lhs, ExprAST *rhs) = 0;
        virtual void visitCallExpr(std::string callee, const std::vector<std::unique_ptr<ExprAST>> &args) = 0;
        virtual void visitPrototype(std::string name, const std::vector<std::string> &args) = 0;
        virtual void visitFunction(PrototypeAST *proto, ExprAST *body) = 0;
};

#endif