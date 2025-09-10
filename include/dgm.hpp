#pragma once
#include <string>
#include <llvm/IR/Module.h>

// === DGM Translator ===
// Takes an LLVM module and writes out a .dgm file
// with 144-opcode mapped instruction stream.
void translateModuleToDGM(llvm::Module &M, const std::string &filename);

// === DGM Emitter ===
// Reads a .dgm file and produces NASM x64 assembly
// ready for `nasm -f win64`.
void emitDGMtoNASM(const std::string &dgmFile, const std::string &nasmFile);
