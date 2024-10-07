#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

namespace {

StringRef LoggerName{"printInstr"};

FunctionType *getLoggerType(LLVMContext &Ctx) {
  Type *RetType = Type::getVoidTy(Ctx);
  PointerType *ArgType = Type::getInt8Ty(Ctx)->getPointerTo();
  return FunctionType::get(RetType, ArrayRef<Type *>{ArgType},
                           false /* isVarArg */);
}

struct InstrTracer final : public PassInfoMixin<InstrTracer> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &FAM) {
    LLVMContext &Ctx = M.getContext();
    FunctionType *LoggerType = getLoggerType(Ctx);
    FunctionCallee FCallee = M.getOrInsertFunction(LoggerName, LoggerType);
    IRBuilder<> Builder(Ctx);

    auto NotLogger = [](const Function &F) {
      return F.getName() != LoggerName;
    };

    auto NotPHI = [](const Instruction &I) {
      return dyn_cast<PHINode>(&I) == nullptr;
    };

    // no need to skip declarations, because if an llvm::Function is a
    // declaration, then it's empty (based on the implementation of
    // GlobalValue::isDeclaration())
    for (auto &F : make_filter_range(M, NotLogger)) {
      for (auto &BB : F) {
        for (auto &I : make_filter_range(BB, NotPHI)) {
          Builder.SetInsertPoint(&I);
          Constant *OpcodeName =
              Builder.CreateGlobalStringPtr(I.getOpcodeName());
          Builder.CreateCall(FCallee, ArrayRef<Value *>{OpcodeName});
        }
      }
    }

    return PreservedAnalyses::none();
  }

  static bool isRequired() { return true; }
};

} // unnamed namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  auto callback = [](PassBuilder &PB) {
    PB.registerPipelineStartEPCallback(
        [](ModulePassManager &MPM, [[maybe_unused]] OptimizationLevel) {
          MPM.addPass(InstrTracer{});
          return true;
        });
  };

  return {LLVM_PLUGIN_API_VERSION, "Instruction tracer", "0.0.1", callback};
}
