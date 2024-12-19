# LLVM Exp3 Loop Info Extraction

The HI_LoopInFormationCollect pass is mainlyed tested by LLVM_exp3_loop_info_extraction

HI_LoopInFormationCollect pass try to extract and print out different Loop information by involving LoopInfoWrapperPass, e.g. Trip Count, Depth, LoopReport, Sub-Loops, Blocks in Loops

It shows how to use the Loop class and LoopInfo class for processing and analysis.
It also shows how to use function getAnalysisUsage to define the dependence between Passes

The test can be run with the following command:

    ./LLVM_expXXXXX  <C/C++ FILE>