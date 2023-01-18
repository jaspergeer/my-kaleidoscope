#include "ast.h"
#include "error.h"
#include "codegen.h"

#include <string>
#include <vector>

#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"

using namespace llvm;

CodeGenerator::CodeGenerator(std::string moduleID) {
    context = std::make_unique<LLVMContext>();
    module = std::make_unique<Module>(moduleID, *context);
    builder = std::make_unique<IRBuilder<>>(*context);
}

// // LLVM IR Generation

void CodeGenerator::logErrorV(const char *str) {
    logError(str);
    valStack.push(nullptr);
}

void CodeGenerator::logErrorF(const char *str) {
    logError(str);
    funStack.push(nullptr);
}

void CodeGenerator::visitNumberExpr(double val) {
    valStack.push(ConstantFP::get(*context, APFloat(val)));
}

void CodeGenerator::visitVariableExpr(std::string name) {
    Value *v = namedValues[name];
    if (!v)
        logError("Unknown variable name");
    valStack.push(v);
}

void CodeGenerator::visitBinaryExpr(char op, ExprAST *lhs, ExprAST *rhs) {
    lhs->accept(this);
    rhs->accept(this);

    llvm::Value *r = valStack.top();
    valStack.pop();
    llvm::Value *l = valStack.top();
    valStack.pop();

    if (!l || !r) {
        valStack.push(nullptr);
        return;
    }

    switch(op) {
        case '+':
            valStack.push(builder->CreateFAdd(l, r, "addtmp"));
            return;
        case '-':
            valStack.push(builder->CreateFSub(l, r, "subtmp"));
            return;
        case '*':
            valStack.push(builder->CreateFMul(l, r, "multmp"));
            return;
        case '<':
            l = builder->CreateFCmpULT(l, r, "cmptmp");
            valStack.push(builder->CreateUIToFP(l, llvm::Type::getDoubleTy(*context), "booltmp"));
            return;
        default:
            logErrorV("invalid binary operator");
    }
}

void CodeGenerator::visitCallExpr(std::string callee, const std::vector<std::unique_ptr<ExprAST>> &args) {
    llvm::Function *calleeF = module->getFunction(callee);
    if (!calleeF) {
        logErrorV("Unknown function referenced");
        return;
    }
        

    if (calleeF->arg_size() != args.size()) {
        logErrorV("Incorrect # arguments passed");
        return;
    }

    std::vector<llvm::Value *> argsV;
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
        args[i]->accept(this);
        argsV.push_back(valStack.top());
        valStack.pop();
        if (!argsV.back())
            valStack.push(nullptr);
            return;
    }

    valStack.push(builder->CreateCall(calleeF, argsV, "calltmp"));
}

void CodeGenerator::visitPrototype(std::string name, const std::vector<std::string> &args) {
    std::vector<Type *> doubles(args.size(), Type::getDoubleTy(*context));

    FunctionType *ft = FunctionType::get(Type::getDoubleTy(*context), doubles, false);

    Function *f = Function::Create(ft, Function::ExternalLinkage, name, module.get());

    unsigned idx = 0;
    for (auto &arg : f->args())
        arg.setName(args[idx++]);

    funStack.push(f);
}

#include <iostream>

void CodeGenerator::visitFunction(PrototypeAST *proto, ExprAST *body) {
    Function *f = module->getFunction(proto->getName());

    // TODO fix diff param names bug???

    if (!f) {
        proto->accept(this);
        f = funStack.top();
        funStack.pop();
    }

    if (!f) {
        funStack.push(nullptr);
        return;
    }

    if (!f->empty()) {
        std::cout << "redefined" << std::endl;
        logErrorF("Function cannot be redefined.");
    }
    
    BasicBlock *bb = BasicBlock::Create(*context, "entry", f);
    builder->SetInsertPoint(bb);

    namedValues.clear();
    for (auto &arg : f->args())
        namedValues[arg.getName().str()] = &arg;

    body->accept(this);
    Value *retVal = valStack.top();
    valStack.pop();

    if (retVal) {
        builder->CreateRet(retVal);
        verifyFunction(*f);
        funStack.push(f);
        return;
    }

    f->eraseFromParent();
    funStack.push(nullptr);
}

Function *CodeGenerator::codegen(DeclAST *ast) {
    ast->accept(this);
    Function *result = funStack.top();
    funStack.pop();
    return result;
}

Value *CodeGenerator::codegen(ExprAST *ast) {
    ast->accept(this);
    Value *result = valStack.top();
    valStack.pop();
    return result;
}