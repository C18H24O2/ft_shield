/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Register.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kiroussa <oss@xtrm.me>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 23:25:40 by kiroussa          #+#    #+#             */
/*   Updated: 2025/06/10 00:28:48 by kiroussa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <llvm/Passes/PassPlugin.h>
#include <llvm/Passes/PassBuilder.h>
#include "obfuscation/BogusControlFlow.h"
#include "obfuscation/Flattening.h"
#include "obfuscation/Split.h"
#include "obfuscation/Substitution.h"
#include "obfuscation/StringObfuscation.h"
#include <iostream>

using namespace llvm;

static cl::opt<std::string> AesSeed("aesSeed", cl::init(""), cl::desc("seed for the AES-CTR PRNG"));

#ifdef _WIN32
#define SP_EXP __declspec(dllexport)
#else
#define SP_EXP
#endif

SP_EXP extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
    llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "shitass-poopface", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
			std::cout << ":trollface:" << std::endl;
			PB.registerVectorizerStartEPCallback(
				[](FunctionPassManager &FPM, OptimizationLevel level) {
					if(!AesSeed.empty()) {
						if(!llvm::cryptoutils->prng_seed(AesSeed.c_str())) {
							exit(1);
						}
					}
					FPM.addPass(BogusControlFlowPass());
					FPM.addPass(SplitBasicBlockPass());
					FPM.addPass(FlatteningPass());
					FPM.addPass(SubstitutionPass());
				}
			);
			PB.registerScalarOptimizerLateEPCallback(
				[](FunctionPassManager &FPM, OptimizationLevel level) {
					FPM.addPass(SubstitutionPass());
				}
			);
			PB.registerOptimizerLastEPCallback(
				[](ModulePassManager &MPM, OptimizationLevel level) {
					MPM.addPass(StringObfuscationNewPass());
				}
			);
        }
	};
}
