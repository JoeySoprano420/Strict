#include "dgm.hpp"
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <fstream>
#include <unordered_map>

// === LLVM → DGM mapping table (subset, can be extended to full 144) ===
static std::unordered_map<unsigned, std::string> DGMTable = {
    { llvm::Instruction::Add,  "17 add" },
    { llvm::Instruction::Sub,  "18 sub" },
    { llvm::Instruction::Mul,  "19 mul" },
    { llvm::Instruction::SDiv, "1B sdiv" },
    { llvm::Instruction::ICmp, "15 icmp" },
    { llvm::Instruction::Br,   "30 br" },
    { llvm::Instruction::Call, "2B call" },
    { llvm::Instruction::Ret,  "33 ret" },
    { llvm::Instruction::Alloca,"01 alloca" },
    { llvm::Instruction::Load,  "02 load" },
    { llvm::Instruction::Store, "03 store" }
};

// === Translator ===
void translateModuleToDGM(llvm::Module &M, const std::string &filename) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        llvm::errs() << "Error: could not open " << filename << " for writing\n";
        return;
    }

    for (auto &F : M) {
        if (F.isDeclaration()) continue;
        out << "FUNC " << F.getName().str() << "\n";

        for (auto &BB : F) {
            out << "  BLOCK " << BB.getName().str() << "\n";
            for (auto &I : BB) {
                unsigned opcode = I.getOpcode();
                if (DGMTable.count(opcode)) {
                    out << "    " << DGMTable[opcode];
                } else {
                    out << "    ?? (" << I.getOpcodeName() << ")";
                }

                // Print operands for readability
                out << " ; ";
                for (unsigned i = 0; i < I.getNumOperands(); i++) {
                    if (auto *op = I.getOperand(i)) {
                        if (op->hasName())
                            out << op->getName().str();
                        else
                            out << "%tmp";
                    }
                    if (i + 1 < I.getNumOperands()) out << ", ";
                }
                out << "\n";
            }
        }
        out << "END FUNC\n\n";
    }

    out.close();
}
