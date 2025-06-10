#include <sstream>
#include <string>

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include "CryptoUtils.h"
#include "StringObfuscation.h"

using namespace llvm;

static cl::opt<bool> StringObf("sobf", cl::init(true),
                               cl::desc("Enable the string obfuscation"));

#define DEBUG_TYPE "objdiv"

STATISTIC(GlobalsEncoded, "Counts number of global variables encoded");

namespace obf_inner {
void flatten(Function *f);
void substitution(Function *f);
void split(Function *f);
void bogus(Function *f);
} // namespace obf_inner

namespace llvm {

struct encVar {
public:
  GlobalVariable *var;
  uint8_t key_xor;
  uint8_t key_cesar;
};

class StringObfuscationPass : public llvm::ModulePass {
public:
  static char ID; // pass identification
  bool is_flag = false;
  StringObfuscationPass() : ModulePass(ID) {}
  StringObfuscationPass(bool flag) : ModulePass(ID) { is_flag = flag; }

  virtual bool runOnModule(Module &M) {
    if (!is_flag)
      return false;
    std::vector<GlobalVariable *> toDelConstGlob;
    // std::vector<GlobalVariable*> encGlob;
    std::vector<encVar *> encGlob;
    for (Module::global_iterator gi = M.global_begin(), ge = M.global_end();
         gi != ge; ++gi) {
      // Loop over all global variables
      GlobalVariable *gv = &(*gi);
      // errs() << "Global var " << gv->getName();
      //       std::string::size_type str_idx =
      //       gv->getName().str().find(".str.");
      std::string section(gv->getSection());

      // Let's encode the static ones
      if (gv->getName().str().substr(0, 4) == ".str" && gv->isConstant() &&
          gv->hasInitializer() &&
          isa<ConstantDataSequential>(gv->getInitializer()) &&
          section != "llvm.metadata" &&
          section.find("__objc_methname") == std::string::npos
          /*&&gv->getType()->getArrayElementType()->getArrayElementType()->isIntegerTy()*/) {
        ++GlobalsEncoded;
        // errs() << " is constant";

        // Duplicate global variable
        GlobalVariable *dynGV = new GlobalVariable(
            M, gv->getValueType(), !(gv->isConstant()), gv->getLinkage(),
            (Constant *)0, gv->getName(), (GlobalVariable *)0,
            gv->getThreadLocalMode(), gv->getType()->getAddressSpace());
        // dynGV->copyAttributesFrom(gv);
        dynGV->setInitializer(gv->getInitializer());

        Constant *initializer = gv->getInitializer();
        ConstantDataSequential *cdata =
            dyn_cast<ConstantDataSequential>(initializer);
        if (cdata) {
          const char *orig = cdata->getRawDataValues().data();
          unsigned len = cdata->getNumElements() * cdata->getElementByteSize();

          encVar *cur = new encVar();
          cur->var = dynGV;
          cur->key_xor = llvm::cryptoutils->get_uint8_t();
          if (cur->key_xor > 0x7F)
            cur->key_xor /= 2;
          cur->key_cesar = llvm::cryptoutils->get_uint8_t();
          if (cur->key_cesar > 0x7F)
            cur->key_cesar /= 2;
          // casting away const is undef. behavior in C++
          // TODO a clean implementation would retrieve the data, generate a new
          // constant set the correct type, and copy the data over.
          // char *encr = new char[len];
          // Constant *initnew = ConstantDataArray::getString(M.getContext(),
          // encr, true);
          char *encr = const_cast<char *>(orig);
          // Simple xor encoding
          for (unsigned i = 0; i != len; ++i) {
            encr[i] = orig[i] ^ cur->key_xor;
            encr[i] += cur->key_cesar;
          }

          // FIXME Second part of the unclean hack.
          dynGV->setInitializer(initializer);

          // Prepare to add decode function for this variable
          encGlob.push_back(cur);
        } else {
          // just copying default initializer for now
          dynGV->setInitializer(initializer);
        }

        // redirect references to new GV and remove old one
        gv->replaceAllUsesWith(dynGV);
        toDelConstGlob.push_back(gv);
      }
    }

    // actuallte delete marked globals
    for (unsigned i = 0, e = toDelConstGlob.size(); i != e; ++i)
      toDelConstGlob[i]->eraseFromParent();

    addDecodeFunction(&M, &encGlob);

    return true;
  }

private:
  void addDecodeFunction(Module *mod, std::vector<encVar *> *gvars) {
    // Declare and add the function definition
    // errs()<<"Successful enter decode function"<<"\n";
    std::vector<Type *> FuncTy_args;
    FunctionType *FuncTy = FunctionType::get(
        /*Result=*/Type::getVoidTy(mod->getContext()), // returning void
        /*Params=*/FuncTy_args,                        // taking no args
        /*isVarArg=*/false);
    uint64_t StringObfDecodeRandomName = cryptoutils->get_uint64_t();
    std::string random_str;
    std::stringstream random_stream;
    random_stream << StringObfDecodeRandomName;
    random_stream >> random_str;
    StringObfDecodeRandomName++;

    auto funccallee =
        mod->getOrInsertFunction(".datadiv_decode" + random_str, FuncTy);
    llvm::Value *c = funccallee.getCallee();

    Function *fdecode = cast<Function>(c);
    fdecode->setCallingConv(CallingConv::C);

    BasicBlock *entry = BasicBlock::Create(mod->getContext(), "entry", fdecode);

    IRBuilder<> builder(mod->getContext());
    builder.SetInsertPoint(entry);

    for (unsigned i = 0, e = gvars->size(); i != e; ++i) {
      GlobalVariable *gvar = (*gvars)[i]->var;
      uint8_t key_xor = (*gvars)[i]->key_xor;
      uint8_t key_cesar = (*gvars)[i]->key_cesar;

      Constant *init = gvar->getInitializer();
      ConstantDataSequential *cdata = dyn_cast<ConstantDataSequential>(init);

      unsigned len = cdata->getNumElements() * cdata->getElementByteSize();
      --len;

      BasicBlock *preHeaderBB = builder.GetInsertBlock();
      BasicBlock *for_body =
          BasicBlock::Create(mod->getContext(), "for-body", fdecode);
      BasicBlock *for_end =
          BasicBlock::Create(mod->getContext(), "for-end", fdecode);
      builder.CreateBr(for_body);
      builder.SetInsertPoint(for_body);
      PHINode *variable =
          builder.CreatePHI(Type::getInt32Ty(mod->getContext()), 2, "i");
      Value *startValue = builder.getInt32(0);
      Value *endValue = builder.getInt32(len);
      variable->addIncoming(startValue, preHeaderBB);
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      // LoadInst *Load=builder.CreateLoad(gvar);
      // errs()<<"Load: "<<*(Load->getPointerOperand())<<"\n";
      Value *indexList[2] = {ConstantInt::get(variable->getType(), 0),
                             variable};
      Value *const_key_cesar = builder.getInt8(key_cesar);
      {
        Value *GEP =
            builder.CreateGEP(gvar->getValueType(), gvar,
                              ArrayRef<Value *>(indexList, 2), "arrayIdx");
        LoadInst *loadElement =
            builder.CreateAlignedLoad(GEP->getType(), GEP, Align(1), false);
        Value *Cesar = builder.CreateSub(loadElement, const_key_cesar, "cesar");
        builder.CreateAlignedStore(Cesar, GEP, Align(1), false);
      }
      Value *const_key_xor = builder.getInt8(key_xor);
      {
        Value *GEP =
            builder.CreateGEP(gvar->getValueType(), gvar,
                              ArrayRef<Value *>(indexList, 2), "arrayIdx");
        LoadInst *loadElement =
            builder.CreateAlignedLoad(GEP->getType(), GEP, Align(1), false);
        // errs()<<"Type: "<<*loadElement<<"\n";
        // CastInst* extended = new ZExtInst(const_key, loadElement->getType(),
        // "extended", for_body); Value* extended =
        // builder.CreateZExtOrBitCast(const_key,
        // loadElement->getType(),"extended");
        Value *Xor = builder.CreateXor(loadElement, const_key_xor, "xor");
        builder.CreateAlignedStore(Xor, GEP, Align(1), false);
      }

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      Value *stepValue = builder.getInt32(1);
      Value *nextValue = builder.CreateAdd(variable, stepValue, "next-value");
      Value *endCondition =
          builder.CreateICmpULT(variable, endValue, "end-condition");
      endCondition = builder.CreateICmpNE(endCondition, builder.getInt1(0),
                                          "loop-condition");
      BasicBlock *loopEndBB = builder.GetInsertBlock();
      builder.CreateCondBr(endCondition, loopEndBB, for_end);
      builder.SetInsertPoint(for_end);
      variable->addIncoming(nextValue, loopEndBB);
    }
    builder.CreateRetVoid();
    appendToGlobalCtors(*mod, fdecode, 0);
    obf_inner::flatten(fdecode);
    obf_inner::bogus(fdecode);
    outs() << "Apply string obfuscation for " << mod->getName() << "\n";
  }
};

} // namespace llvm

llvm::PreservedAnalyses
llvm::StringObfuscationNewPass::run(llvm::Module &M,
                                    llvm::ModuleAnalysisManager &AM) {
  (void) AM;
  llvm::StringObfuscationPass sobf(StringObf);
  if (sobf.runOnModule(M))
    return PreservedAnalyses::all();

  return PreservedAnalyses::none();
}

char StringObfuscationPass::ID = 0;
static RegisterPass<StringObfuscationPass>
    X("GVDiv", "Global variable (i.e., const char*) diversification pass",
      false, true);

Pass *llvm::createStringObfuscation(bool flag) {
  return new StringObfuscationPass(flag);
}
