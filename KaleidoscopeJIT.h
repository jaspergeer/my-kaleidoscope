#ifndef KALEIDOSCOPEJIT_H
#define KALEIDOSCOPEJIT_H

#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include <memory>

namespace llvm {
namespace orc {

class KaleidoscopeJIT {
private:
    std::unique_ptr<ExecutionSession> es;
    RTDyldObjectLinkingLayer objectLayer;
    IRCompileLayer compileLayer;

    DataLayout dl;
    MangleAndInterner mangle;
    
    JITDylib &mainJD;

public:
    KaleidoscopeJIT(std::unique_ptr<ExecutionSession> es,
        JITTargetMachineBuilder jtmb, DataLayout dl);
    ~KaleidoscopeJIT();
    static Expected<std::unique_ptr<KaleidoscopeJIT>> Create();
    const DataLayout &getDataLayout() const;
    JITDylib &getMainJITDylib();
    Error addModule(ThreadSafeModule tsm, ResourceTrackerSP rt = nullptr);
    Expected<JITEvaluatedSymbol> lookup(StringRef name);
};

}
}

#endif