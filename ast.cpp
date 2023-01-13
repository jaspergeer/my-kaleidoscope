#include "ast.h"
#include "visitor.h"

void NumberExprAST::accept(ASTVisitor &v) {
    v.visitNumberExpr(val);
}

void VariableExprAST::accept(ASTVisitor &v) {
    v.visitVariableExpr(name);
}

void BinaryExprAST::accept(ASTVisitor &v) {
    v.visitBinaryExpr(op, lhs.get(), rhs.get());
}

void CallExprAST::accept(ASTVisitor &v) {
    v.visitCallExpr(callee, args);
}

void PrototypeAST::accept(ASTVisitor &v) {
    v.visitPrototype(name, args);
}

void FunctionAST::accept(ASTVisitor &v) {
    v.visitFunction(proto.get(), body.get());
}