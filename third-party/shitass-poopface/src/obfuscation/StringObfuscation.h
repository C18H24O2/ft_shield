// LLVM include
#include "llvm/Pass.h"
#include "llvm/IR/PassManager.h" // New PassManager

namespace llvm {
      Pass* createStringObfuscation(bool flag);
class StringObfuscationNewPass : public PassInfoMixin<StringObfuscationNewPass>{ // New PassManager
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);

  static bool isRequired() { return true; }
};
}
