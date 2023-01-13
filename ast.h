#ifndef AST_H
#define AST_H

#include <string>
#include <map>

#include "llvm/IR/Value.h"

class ASTVisitor;

class ExprAST {
public:
    virtual ~ExprAST() {}
    virtual void accept(ASTVisitor &v) = 0;
};

class NumberExprAST : public ExprAST {
    const double val;

public:
    NumberExprAST(double val) : val(val) {}
    void accept(ASTVisitor &v) override;
};

class VariableExprAST : public ExprAST {
    const std::string name;

public:
    VariableExprAST(const std::string &name) : name(name) {}
    void accept(ASTVisitor &v) override;
};

class BinaryExprAST : public ExprAST {
    const char op;
    const std::unique_ptr<ExprAST> lhs, rhs;
public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs) : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    void accept(ASTVisitor &v) override;
};

class CallExprAST : public ExprAST {
    const std::string callee;
    const std::vector<std::unique_ptr<ExprAST>> args;
public:
    CallExprAST(const std::string &callee, std::vector<std::unique_ptr<ExprAST>> args) : callee(callee), args(std::move(args)) {}
    void accept(ASTVisitor &v) override;
};

class PrototypeAST {
    const std::string name;
    const std::vector<std::string> args;
public:
    PrototypeAST(const std::string &name, std::vector<std::string> args) : name(name), args(std::move(args)) {}
    void accept(ASTVisitor &v);
};

class FunctionAST {
    const std::unique_ptr<PrototypeAST> proto;
    const std::unique_ptr<ExprAST> body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> body) : proto(std::move(proto)), body(std::move(body)) {}
    void accept(ASTVisitor &v);
};

#endif