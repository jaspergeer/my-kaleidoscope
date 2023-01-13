#include "visitor.h"
#include "llvm/IR/Value.h"

#include <stack>

namespace llvm {

class ASTCodeGen : public ASTVisitor {
    private:
        LLVMContext context;
        IRBuilder<> builder;
        std::unique_ptr<Module> module;
        std::map<std::string, Value *> namedValues;

        // store intermediate results
        std::stack<Value *> valStack;
        std::stack<Function *> funStack;
    public:
        ASTCodeGen();
        void visitNumberExpr(double val) override;
        void visitVariableExpr(std::string name) override;
        void visitBinaryExpr(char op, ExprAST *lhs, ExprAST *rhs) override;
        void visitCallExpr(std::string callee, const std::vector<std::unique_ptr<ExprAST>> &args) override;
        void visitPrototype(std::string name, const std::vector<std::string> &args) override;
        void visitFunction(PrototypeAST *proto, ExprAST *body) override;
};
}