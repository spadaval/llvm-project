//
// Created by Sumeet Padavala on 8/9/23.
//

#ifndef LLVM_PROJECT_GATEBUILDER_H
#define LLVM_PROJECT_GATEBUILDER_H

#include "llvm/Pass.h"

namespace llvm {
FunctionPass* createGateBuilderLegacyPass();
} // end namespace llvm

#endif // LLVM_PROJECT_GATEBUILDER_H
