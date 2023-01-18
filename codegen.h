#include "visitor.h"

#include "llvm/IR/Value.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"

#include <stack>

using namespace llvm;

class CodeGenerator : public ASTVisitor {
    private:
        std::unique_ptr<LLVMContext> context;
        std::unique_ptr<IRBuilder<>> builder;
        std::unique_ptr<Module> module;
        std::map<std::string, Value *> namedValues;

        void logErrorV(const char *str);
        void logErrorF(const char *str);

        // store intermediate results
        std::stack<Value *> valStack;
        std::stack<Function *> funStack;
    public:
        CodeGenerator(std::string moduleID);
        Function *codegen(DeclAST *ast);
        Value *codegen(ExprAST *ast);

        void visitNumberExpr(double val) override;
        void visitVariableExpr(std::string name) override;
        void visitBinaryExpr(char op, ExprAST *lhs, ExprAST *rhs) override;
        void visitCallExpr(std::string callee, const std::vector<std::unique_ptr<ExprAST>> &args) override;
        void visitPrototype(std::string name, const std::vector<std::string> &args) override;
        void visitFunction(PrototypeAST *proto, ExprAST *body) override;
};