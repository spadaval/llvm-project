#include "llvm/Transforms/Scalar/GateGlobals.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Debug.h"
using namespace llvm;
using namespace std;

#define DEBUG_TYPE "extra-protein"

namespace llvm {
void initializeGateGlobalsLegacyPassPass(PassRegistry &);
} // end namespace llvm

namespace {
struct GateGlobalsLegacyPass : public ModulePass {
  static char ID;
  GateGlobalsLegacyPass() : ModulePass(ID) {
    initializeGateGlobalsLegacyPassPass(*PassRegistry::getPassRegistry());
  }

  bool runOnModule(Module &M) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addRequired<LoopInfoWrapperPass>();
  }
};
} // end anonymous namespace

char GateGlobalsLegacyPass::ID = 42;
INITIALIZE_PASS_BEGIN(GateGlobalsLegacyPass, "gate-builder",
                      "Increase EVERY loops' trip counts! "
                      "(and break your program logic)",
                      false, false)
// INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_END(GateGlobalsLegacyPass, "gate-builder",
                    "Increase EVERY loops' trip counts! "
                    "(and break your program logic)",
                    false, false)

llvm::Function *get_key_start(llvm::Module &M) {
  std::vector<llvm::Type *> arg_types;
  arg_types.push_back(llvm::Type::getInt32Ty(M.getContext()));

  llvm::FunctionType *func_type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(M.getContext()), arg_types, false);

  llvm::Function *func = llvm::Function::Create(
      func_type, llvm::Function::ExternalLinkage, llvm::Twine("key_start"), M);
  func->setCallingConv(llvm::CallingConv::C);
  return func;
}

llvm::Function *get_key_end(llvm::Module &M) {
  std::vector<llvm::Type *> arg_types;
  arg_types.push_back(llvm::Type::getInt32Ty(M.getContext()));

  llvm::FunctionType *func_type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(M.getContext()), arg_types, false);

  llvm::Function *func = llvm::Function::Create(
      func_type, llvm::Function::ExternalLinkage, llvm::Twine("key_end"), M);
  func->setCallingConv(llvm::CallingConv::C);
  return func;
}

llvm::Function *get_gate(llvm::Module &M) {
  std::vector<llvm::Type *> arg_types;
  arg_types.push_back(llvm::Type::getInt32Ty(M.getContext()));

  llvm::FunctionType *func_type = llvm::FunctionType::get(
      llvm::Type::getInt32Ty(M.getContext()), arg_types, false);

  llvm::Function *func = llvm::Function::Create(
      func_type, llvm::Function::ExternalLinkage, llvm::Twine("gate"), M);
  func->setCallingConv(llvm::CallingConv::C);
  return func;
}

bool GateGlobalsLegacyPass::runOnModule(llvm::Module &M) {
//  get_key_start(M);
//  get_key_end(M);
//  get_gate(M);

  // string name = F.getGlobalIdentifier();
  //   string seededName = name + to_string(seed);
  //   unsigned gateId = hasher(seededName);
  //   printf("%d: %s\n", gateId, F.getName().str().c_str());

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
ModulePass *createGateGlobalsLegacyPass() {
  return new GateGlobalsLegacyPass();
}
} // end namespace llvm