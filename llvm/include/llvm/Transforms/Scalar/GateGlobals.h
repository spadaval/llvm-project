//
// Created by Sumeet Padavala on 8/9/23.
//

#ifndef LLVM_PROJECT_GATEGLOBALS_H
#define LLVM_PROJECT_GATEGLOBALS_H

#include "llvm/Pass.h"

namespace llvm {
ModulePass* createGateGlobalsLegacyPass();
} // end namespace llvm

#endif // LLVM_PROJECT_GATEGLOBALS_H
