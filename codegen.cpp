#include "ast.h"
#include "error.h"
#include "codegen.h"

#include <string>
#include <vector>

#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"

namespace llvm {

ASTCodeGen::ASTCodeGen() : builder(context) {}

// // LLVM IR Generation

void ASTCodeGen::visitNumberExpr(double val) {
    valStack.push(ConstantFP::get(context, APFloat(val)));
}

void ASTCodeGen::visitVariableExpr(std::string name) {
    Value *v = namedValues[name];
    if (!v)
        logError("Unknown variable name");
    valStack.push(v);
}

// llvm::Value *BinaryExprAST::codegen() {
//     llvm::Value *l = lhs->codegen();
//     llvm::Value *r = rhs->codegen();
//     if (!l || !r)
//         return nullptr;

//     switch(op) {
//         case '+':
//             return Builder.CreateFAdd(l, r, "addtmp");
//         case '-':
//             return Builder.CreateFSub(l, r, "subtmp");
//         case '*':
//             return Builder.CreateFMul(l, r, "multmp");
//         case '<':
//             l = Builder.CreateFCmpULT(l, r, "cmptmp");
//             return Builder.CreateUIToFP(l, llvm::Type::getDoubleTy(Context), "booltmp");
//         default:
//             return logErrorV("invalid binary operator");
//     }
// }

// llvm::Function *CallExprAST::codegen() {
//     llvm::Function *calleeF = Module->getFunction(callee);
//     if (!calleeF)
//         return logErrorV("Unknown function referenced");

//     if (calleeF->arg_size() != args.size())
//         return logErrorV("Incorrect # arguments passed");

//     std::vector<llvm::Value *> argsV;
//     for (unsigned i = 0, e = args.size(); i != e, ++i) {
//         argsV.push_back(args[i]->codegen());
//         if (!argsV.back())
//             return nullptr;
//     }

//     return Builder.CreateCall(calleeF, argsV, "calltmp");
// }

// llvm::Function *PrototypeAST::codegen() {
//     std::vector<llvm::Type *>
// }
}