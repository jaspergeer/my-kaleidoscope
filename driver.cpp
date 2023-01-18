#include "parser.h"
#include "codegen.h"
#include "error.h"

#include "llvm/IR/Value.h"

using namespace llvm;

static std::unique_ptr<CodeGenerator> cg;

static void InitializeModule() {
    cg = std::make_unique<CodeGenerator>("jasper module");
}

static void HandleDefinition() {
    if (auto FnAST = parseDefinition()) {
        if (auto *FnIR = cg->codegen(FnAST.get())) {
        fprintf(stderr, "Read function definition:\n");
        FnIR->print(errs());
        fprintf(stderr, "\n");
        }
    } else {
        // Skip token for error recovery.
        getNextToken();
    }
}

static void HandleExtern() {
  if (auto ProtoAST = parseExtern()) {
    if (auto *FnIR = cg->codegen(ProtoAST.get())) {
      fprintf(stderr, "Read extern:\n");
      FnIR->print(errs());
      fprintf(stderr, "\n");
    }
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleTopLevelExpression() {
  // Evaluate a top-level expression into an anonymous function.
  if (auto FnAST = parseTopLevelExpr()) {
    if (auto *FnIR = cg->codegen(FnAST.get())) {
      fprintf(stderr, "Read top-level expression:\n");
      FnIR->print(errs());
      fprintf(stderr, "\n");

      // Remove the anonymous expression.
      FnIR->eraseFromParent();
    }
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

/// top ::= definition | external | expression | ';'
static void MainLoop() {
  while (true) {
    fprintf(stderr, "ready> ");
    switch (CurTok) {
    case tok_eof:
      return;
    case ';': // ignore top-level semicolons.
      getNextToken();
      break;
    case tok_def:
      HandleDefinition();
      break;
    case tok_extern:
      HandleExtern();
      break;
    default:
      HandleTopLevelExpression();
      break;
    }
  }
}

//===----------------------------------------------------------------------===//
// Main driver code.
//===----------------------------------------------------------------------===//

int main() {
    // Prime the first token.
    fprintf(stderr, "ready> ");
    getNextToken();

    // Make the module, which holds all the code.
    InitializeModule();

    // Run the main "interpreter loop" now.
    MainLoop();

    // Print out all of the generated code.
    //   TheModule->print(errs(), nullptr);

    return 0;
}