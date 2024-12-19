#include "HI_Polly_Info.h"
#include "HI_print.h"
#include "polly/PolyhedralInfo.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include <ios>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace llvm;

bool HI_Polly_Info::runOnModule(
    Module &M) // The runOnModule declaration will overide the virtual one in ModulePass, which
               // will be executed for each Module.
{
    for (Function &F : M)
    {
        SE = &getAnalysis<ScalarEvolutionWrapperPass>(F).getSE();
        LI = &getAnalysis<LoopInfoWrapperPass>(F).getLoopInfo();
    }
    // auto *LAA = &getAnalysis<LoopAccessLegacyAnalysis>();
    auto PSD = &getAnalysis<polly::ScopDetectionWrapperPass>();
    PSD->print(*Loop_out);
    return false;
}

char HI_Polly_Info::ID =
    0; // the ID for pass should be initialized but the value does not matter, since LLVM uses the
       // address of this variable as label instead of its value.

void HI_Polly_Info::getAnalysisUsage(AnalysisUsage &AU) const
{
    AU.setPreservesAll();
    AU.addRequiredTransitive<LoopInfoWrapperPass>();
    AU.addRequiredTransitive<ScalarEvolutionWrapperPass>();
    // AU.addRequired<LoopAccessLegacyAnalysis>();
    AU.addRequiredTransitive<DominatorTreeWrapperPass>();
    //  AU.addPreserved<DominatorTreeWrapperPass>();
    AU.addRequiredTransitive<OptimizationRemarkEmitterWrapperPass>();
    AU.addRequiredTransitive<polly::DependenceInfoWrapperPass>();
    // AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequiredTransitive<polly::ScopInfoWrapperPass>();
    AU.addRequiredTransitive<polly::PolyhedralInfo>();
    AU.addRequiredTransitive<polly::ScopDetectionWrapperPass>();
    // Ensure all required analyses are declared
    // AU.addPreserved<GlobalsAAWrapperPass>();
}
