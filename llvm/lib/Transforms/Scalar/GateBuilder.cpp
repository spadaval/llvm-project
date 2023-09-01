#include "llvm/Transforms/Scalar/GateBuilder.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

using namespace llvm;
using namespace std;

#define DEBUG_TYPE "extra-protein"

namespace llvm {
void initializeGateBuilderLegacyPassPass(PassRegistry &);
} // end namespace llvm

namespace {
struct GateBuilderLegacyPass : public FunctionPass {
  static char ID;
  GateBuilderLegacyPass() : FunctionPass(ID) {
    initializeGateBuilderLegacyPassPass(*PassRegistry::getPassRegistry());
  }

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addRequired<LoopInfoWrapperPass>();
  }
};
} // end anonymous namespace

char GateBuilderLegacyPass::ID = 14;
INITIALIZE_PASS_BEGIN(GateBuilderLegacyPass, "gate-builder",
                      "Increase EVERY loops' trip counts! "
                      "(and break your program logic)",
                      false, false)
// INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_END(GateBuilderLegacyPass, "gate-builder",
                    "Increase EVERY loops' trip counts! "
                    "(and break your program logic)",
                    false, false)

// TODO make this a CLI arg
int seed = 42;
hash<string> hasher;

void insertKeyStart(Function &F) {
  string name = F.getGlobalIdentifier();
  string seededName = name + to_string(seed);
  unsigned keyId = hasher(seededName);
  printf("Inserting key enter [%d] into function: [%s]\n", keyId,
         F.getName().str().c_str());

  std::vector<llvm::Type *> arg_types;
  arg_types.push_back(llvm::Type::getInt32Ty(F.getContext()));

  llvm::FunctionType *func_type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(F.getContext()), arg_types, false);

  auto funcToInsert =
      F.getParent()->getOrInsertFunction("key_start", func_type);

  vector<Value *> args;
  args.push_back(ConstantInt::get(Type::getInt32Ty(F.getContext()), keyId));

  CallInst::Create(funcToInsert, args, "key_start",
                   &F.front().front()); // Insert call at the beginning
}

void insertKeyEnd(Function &F) {
  string name = F.getGlobalIdentifier();
  string seededName = name + to_string(seed);
  unsigned keyId = hasher(seededName);
  printf("Inserting key exit [%d] into function: [%s]\n", keyId,
         F.getName().str().c_str());

  std::vector<llvm::Type *> arg_types;
  arg_types.push_back(llvm::Type::getInt32Ty(F.getContext()));

  llvm::FunctionType *func_type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(F.getContext()), arg_types, false);

  auto funcToInsert = F.getParent()->getOrInsertFunction("key_end", func_type);

  vector<Value *> args;
  args.push_back(ConstantInt::get(Type::getInt32Ty(F.getContext()), keyId));

  CallInst::Create(funcToInsert, args, "key_end",
                   &F.back().back()); // Insert call at the beginning
}

bool GateBuilderLegacyPass::runOnFunction(Function &F) {

  // insertPrint(F);
  insertKeyStart(F);
  insertKeyEnd(F);

  errs() << "Function: " << F.getName().str() << "[";

  for (auto &attr : F.getAttributes()) {
    errs() << attr.getAsString() << ",";
  }
  errs() << "]\n";

  for (auto &bb : F) {
    for (auto &ins : bb) {
      errs() << ins << "\n";
    }
    errs() << "\n";
  }

  return true;

  //  auto& LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  //  if(LI.empty()) return false;
  //
  //  // (ExitLimit user, ExitLimit)
  //  SmallVector<std::pair<Instruction*,const Value*>, 2> Worklist;
  //  for(Loop* L : LI) {
  //    // Can only handle single exit
  //    auto* ExitingBB = L->getExitingBlock();
  //    if(!ExitingBB) continue;
  //    auto* Br = dyn_cast<BranchInst>(ExitingBB->getTerminator());
  //    if(!Br || Br->getNumSuccessors() != 2) continue;
  //    BasicBlock *TrueBB = Br->getSuccessor(0);
  //    auto* Cmp = dyn_cast<CmpInst>(Br->getCondition());
  //    if(!Cmp) continue;
  //    LLVM_DEBUG(dbgs() << "Exit condition: " << *Cmp << "\n");
  //
  //    // Just a simplified way to find indvar
  //    PHINode *IndVar = nullptr;
  //    // We always want to get form like 'i < C' or 'i > C'
  //    // instead of 'C < i' or 'C > i'
  //    CmpInst::Predicate Pred;
  //    if(auto* PN = dyn_cast<PHINode>(Cmp->getOperand(0))) {
  //      IndVar = PN;
  //      Pred = Cmp->getPredicate();
  //    } else if(auto* PN = dyn_cast<PHINode>(Cmp->getOperand(1))) {
  //      IndVar = PN;
  //      Pred = Cmp->getInversePredicate();
  //    }
  //    if(!IndVar) continue;
  //
  //    auto processAscending = [Pred,&Worklist](CmpInst *Cmp) {
  //      if(Pred != Cmp->getPredicate())
  //        // IndVar is on RHS
  //        Worklist.push_back(std::make_pair(cast<Instruction>(Cmp),
  //                                          Cmp->getOperand(0)));
  //      else
  //        Worklist.push_back(std::make_pair(cast<Instruction>(Cmp),
  //                                          Cmp->getOperand(1)));
  //    };
  //
  //    auto processDescending = [L,&Worklist](PHINode *IndVar) {
  //      unsigned i;
  //      for(i = 0; i < IndVar->getNumIncomingValues(); ++i){
  //        if(L->contains(IndVar->getIncomingBlock(i))) continue;
  //        // Get the initial value
  //        Worklist.push_back(std::make_pair(cast<Instruction>(IndVar),
  //                                          IndVar->getIncomingValue(i)));
  //        break;
  //      }
  //    };
  //
  //    // See whether it's ascending or descending
  //    switch(Pred){
  //    case CmpInst::ICMP_UGT:
  //    case CmpInst::ICMP_UGE:
  //    case CmpInst::ICMP_SGT:
  //    case CmpInst::ICMP_SGE: {
  //      if(L->contains(TrueBB))
  //        // descending
  //        processDescending(IndVar);
  //      else
  //        // ascending
  //        processAscending(Cmp);
  //    }
  //    case CmpInst::ICMP_ULT:
  //    case CmpInst::ICMP_ULE:
  //    case CmpInst::ICMP_SLT:
  //    case CmpInst::ICMP_SLE: {
  //      if(L->contains(TrueBB))
  //        // ascending
  //        processAscending(Cmp);
  //      else
  //        // descending
  //        processDescending(IndVar);
  //    }
  //    default:
  //      continue;
  //    }
  //  }
  //
  //  bool Changed = false;
  //  for(const auto& P : Worklist) {
  //    User* Usr = cast<User>(P.first);
  //    const Value* Val = P.second;
  //    LLVM_DEBUG(dbgs() << *Usr << " -> " << *Val << "\n");
  //    auto* Ty = Val->getType();
  //    if(!Ty->isIntegerTy()) continue;
  //
  //    Value *NewVal;
  //    if(const auto* ConstInt = dyn_cast<ConstantInt>(Val)){
  //      NewVal = ConstantInt::get(Ty,
  //                                ConstInt->getValue() * 2);
  //    }else{
  //      // Non constant, insert multiplication
  //      if(!isa<Instruction>(Usr)) continue;
  //      auto* Two = ConstantInt::get(Ty, 2);
  //      NewVal = BinaryOperator::Create(BinaryOperator::Mul,
  //                                      const_cast<Value*>(Val), Two, "",
  //                                      cast<Instruction>(Usr));
  //    }
  //    Changed = true;
  //    Usr->replaceUsesOfWith(const_cast<Value*>(Val), NewVal);
  //  }
  //
  //  return Changed;
}

namespace llvm {
FunctionPass *createGateBuilderLegacyPass() {
  return new GateBuilderLegacyPass();
}
} // end namespace llvm