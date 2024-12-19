# LLVM Exp5a InstructionList

mainly test the HI_SimpleTimingEvaluation pass


The pass traverses the top_function, including the blocks he subfunctions and loops to get the total latency, according to the CFG (predecessor-successor)
The latency is obtained by ASAP scheduling according to the dependence between blocks. (detailed implementation and explanation can be found in source code.)

the pass handles the module block by block, with the help of many subfunctions of different LLVM pre-defined classes.


The test can be run with the following command:

      ./LLVM_expXXXXX  <C/C++ FILE> <top_function_name>   
      

Implementation idea is shown below:

if the block is in a loop, the latency of the entire loop will be calculated. After that, all the blocks will be merged into a latency node for later processing
for example, if some blocks is successed by the loop, when the pass traverses to the block in loop, it will directly jump the the exiting blocks and accumulate the loop latency

for subfunctions, if the pass traverses to CallInstruction in the blocks, it will first get the latency of the subfucntion and assign the latency to the CallInstruction.


Related LLVM Passes:
LoopSimplify 
IndVarSimplifyPass --- createIndVarSimplifyPass
LoopStrengthReducePass --- createLoopStrengthReducePass