//===- SubstitutionIncludes.h - Substitution Obfuscation ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains includes and defines for the substitution pass
//
//===----------------------------------------------------------------------===//

#ifndef _OBFUSCATION_SUBSTITUTIONS_H_
#define _OBFUSCATION_SUBSTITUTIONS_H_

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/IR/PassManager.h"
#include "CryptoUtils.h"

namespace llvm {

Pass *createSubstitution();
class SubstitutionPass : public PassInfoMixin<SubstitutionPass>{
	public:
		PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);

		static bool isRequired() { return true; }
};

} // namespace llvm

#endif
