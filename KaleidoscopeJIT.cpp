#include "KaleidoscopeJIT.h"

namespace llvm {
namespace orc {

KaleidoscopeJIT::KaleidoscopeJIT(std::unique_ptr<ExecutionSession> es,
        JITTargetMachineBuilder jtmb, DataLayout dl)
    : es(std::move(es)),
    dl(std::move(dl)), mangle(*this->es, this->dl),
    objectLayer(*this->es,
        []() { return std::make_unique<SectionMemoryManager>(); }),
    compileLayer(*this->es, objectLayer,
        std::make_unique<ConcurrentIRCompiler>(std::move(jtmb))),
    mainJD(this->es->createBareJITDylib("<main>")) {
        mainJD.addGenerator(
            cantFail(DynamicLibrarySearchGenerator::GetForCurrentProcess(
                dl.getGlobalPrefix())));
}

KaleidoscopeJIT::~KaleidoscopeJIT() {
    if (auto err = es->endSession())
        es->reportError(std::move(err));
}

Expected<std::unique_ptr<KaleidoscopeJIT>> KaleidoscopeJIT::Create() {
    auto epc = SelfExecutorProcessControl::Create();
    if (!epc)
        return epc.takeError();

    auto es = std::make_unique<ExecutionSession>(std::move(*epc));

    JITTargetMachineBuilder jtmb(
        es->getExecutorProcessControl().getTargetTriple());
    
    auto dl = jtmb.getDefaultDataLayoutForTarget();
    if (!dl)
        return dl.takeError();

    return std::make_unique<KaleidoscopeJIT>(std::move(es), std::move(jtmb),
                                                            std::move(*dl));
}

const DataLayout &KaleidoscopeJIT::getDataLayout() const { return dl; }

JITDylib &KaleidoscopeJIT::getMainJITDylib() { return mainJD; }

Error KaleidoscopeJIT::addModule(ThreadSafeModule tsm, ResourceTrackerSP rt = nullptr) {
    if (!rt)
        rt = mainJD.getDefaultResourceTracker();
    return compileLayer.add(rt, std::move(tsm));
}

Expected<JITEvaluatedSymbol> KaleidoscopeJIT::lookup(StringRef name) {
    return es->lookup({&mainJD}, mangle(name.str()));
}

}
}