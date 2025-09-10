#pragma once
#include <string>
#include <llvm/IR/Module.h>

// Translate LLVM IR to DGM opcode stream
void translateModuleToDGM(llvm::Module &M, const std::string &filename);

// Emit NASM x64 assembly from DGM file
void emitDGMtoNASM(const std::string &dgmFile, const std::string &nasmFile);
