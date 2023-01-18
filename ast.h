#ifndef AST_H
#define AST_H

#include <string>
#include <map>
#include <vector>

#include "llvm/IR/Value.h"

class ASTVisitor;

class AST {
public:
    virtual ~AST() {}
    virtual void accept(ASTVisitor *v) = 0;
};

class ExprAST : public AST {};

class NumberExprAST : public ExprAST {
    double val;

public:
    NumberExprAST(double val) : val(val) {}
    void accept(ASTVisitor *v) override;
};

class VariableExprAST : public ExprAST {
    std::string name;

public:
    VariableExprAST(const std::string &name) : name(name) {}
    void accept(ASTVisitor *v) override;
};

class BinaryExprAST : public ExprAST {
    char op;
    std::unique_ptr<ExprAST> lhs, rhs;
public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    void accept(ASTVisitor *v) override;
};

class CallExprAST : public ExprAST {
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args;
public:
    CallExprAST(const std::string &callee, std::vector<std::unique_ptr<ExprAST>> args) : callee(callee), args(std::move(args)) {}
    void accept(ASTVisitor *v) override;
};

class DeclAST : public AST {
public:
    void accept(ASTVisitor *v) = 0;
};

class PrototypeAST : public DeclAST {
    std::string name;
    std::vector<std::string> args;
public:
    PrototypeAST(const std::string &name, std::vector<std::string> args) : name(name), args(std::move(args)) {}
    void accept(ASTVisitor *v) override;
    std::string getName();
};

class FunctionAST : public DeclAST {
    std::unique_ptr<PrototypeAST> proto;
    std::unique_ptr<ExprAST> body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> body) : proto(std::move(proto)), body(std::move(body)) {}
    void accept(ASTVisitor *v) override;
};

#endif