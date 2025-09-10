#pragma once
#include <string>
#include <llvm/IR/Value.h>

struct ProgramAST;

// === Codegen API ===

// Generate LLVM IR for the entire program
// (implemented in src/codegen_llvm.cpp)
llvm::Value* codegenProgram(ProgramAST &program);

// Emit LLVM IR to a file (.ll)
void emitIR(ProgramAST &program, const std::string &filename);
