# LLVM Exp1 Dependence List

The HI_DependenceList pass is mainlyed tested by LLVM_exp1_dependence_list

The test can be run with the following command:

    ./LLVM_expXXXXX  <C/C++ FILE>

HI_DependenceList pass extracts the dependence between Instructions. Detailed explanation can be found in the source code's comments.

To implement this pass, different iterators are used in to iterate functions, blocks and instrucions (and their successors(users))

e.g.


for (User *U : (I)->users())

for (auto Succ_it : successors(B))

for (Instruction &I: B) 

for (BasicBlock &B : F) 