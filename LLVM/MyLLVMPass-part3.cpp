//===- MyLLVMPass.cpp - Example code from "Writing an LLVM Pass" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "hello"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>
#include <algorithm>

#include "llvm/IR/IRBuilder.h"

using namespace llvm;

StringRef fname;

namespace
{
// Hello - The first implementation, without getAnalysisUsage.
struct MyLLVMPass : public ModulePass
{
  static char ID; // Pass identification, replacement for typeid

  MyLLVMPass() : ModulePass(ID)
  {
  }

  virtual bool runOnModule(Module &M)
  {
    bool changed = false;
    errs() << "Part 3:"
           << "\n\n"; // Test code

    Function *logFunc = M.getFunction("log_msg");
    Function *countFunc = M.getFunction("log_call_count");
    GlobalVariable *gvar_int32_i = new GlobalVariable(
        M,
        IntegerType::get(M.getContext(), 32),
        false,
        GlobalVariable::ExternalLinkage,
        0,
        "i");
    /*
    GlobalVariable(Module &M, Type *Ty, bool isConstant,
                 LinkageTypes Linkage, Constant *Initializer,
                 const Twine &Name = "", GlobalVariable *InsertBefore = 0,
                 ThreadLocalMode = NotThreadLocal, unsigned AddressSpace = 0,
                 bool isExternallyInitialized = false);
    */
    ConstantInt *const_int32_2 = ConstantInt::get(M.getContext(), APInt(32, StringRef("0"), 10));
    gvar_int32_i->setInitializer(const_int32_2);

    for (Function &fun : M)
    {
      if (fun.getName() == "log_msg" || fun.getName() == "log_call_count")
      {
        continue;
      }
      for (BasicBlock &bb : fun)
      {
        for (Instruction &i : bb)
        {
          // If it's a call
          if (auto *ci = dyn_cast<CallInst>(&i))
          {
            IRBuilder<> builder(ci);

            // Count function call
            LoadInst *Load = builder.CreateLoad(gvar_int32_i);
            Value *Inc = builder.CreateAdd(builder.getInt32(1), Load);
            builder.CreateStore(Inc, gvar_int32_i);

            // Write in file
            std::string str = (fun.getName() + " --> " + ci->getCalledFunction()->getName() + " (@" + ci->getName() + ")").str();
            Value *formatStr = builder.CreateGlobalStringPtr(str);
            builder.CreateCall(logFunc, formatStr);
          }

          // If it's a return
          if (auto *ci = dyn_cast<ReturnInst>(&i))
          {
            if (fun.getName() == "main")
            {
              IRBuilder<> builder(ci);

              // Write in file
              Value *val = builder.CreateLoad(gvar_int32_i);
              builder.CreateCall(countFunc, val);
            }
          }
        }
      }
    }

    return changed;
  }
};
} // namespace

char MyLLVMPass::ID = 0;
static RegisterPass<MyLLVMPass> X("MyLLVMPass", "MyLLVMPass");
