#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "codegen.hpp"
#include "dgm.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

// Helper: run a shell command
int runCommand(const std::string &cmd) {
    std::cout << ">> " << cmd << "\n";
#ifdef _WIN32
    return system(cmd.c_str());
#else
    return system(cmd.c_str());
#endif
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: strictc <file.strict> [-o output.exe]\n";
        return 1;
    }

    std::string inputFile = argv[1];
    std::string baseName = inputFile.substr(0, inputFile.find_last_of('.'));
    std::string outFile = baseName + ".exe";

    // Allow -o flag
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "-o" && i + 1 < argc) {
            outFile = argv[i + 1];
            i++;
        }
    }

    // 1. Read source file
    std::ifstream src(inputFile);
    if (!src.is_open()) {
        std::cerr << "Error: cannot open " << inputFile << "\n";
        return 1;
    }
    std::string source((std::istreambuf_iterator<char>(src)),
                        std::istreambuf_iterator<char>());

    // 2. Lex & Parse
    Lexer lex(source);
    Parser parser(lex);
    ProgramAST program = parser.parseProgram();

    // Debug: print AST
    // program.print();

    // 3. Generate LLVM IR
    std::string llFile = baseName + ".ll";
    program.codegen();
    program.emitIR(llFile);

    std::cout << "Generated LLVM IR: " << llFile << "\n";

    // 4. Translate to DGM
    std::string dgmFile = baseName + ".dgm";
    translateModuleToDGM(*program.getModule(), dgmFile);
    std::cout << "Generated DGM: " << dgmFile << "\n";

    // 5. Emit NASM
    std::string nasmFile = baseName + ".s";
    emitDGMtoNASM(dgmFile, nasmFile);
    std::cout << "Generated NASM: " << nasmFile << "\n";

    // 6. Assemble with NASM
    std::string objFile = baseName + ".obj";
    std::string nasmCmd = "nasm -f win64 " + nasmFile + " -o " + objFile;
    if (runCommand(nasmCmd) != 0) {
        std::cerr << "Error: NASM failed.\n";
        return 1;
    }

    // 7. Link with MSVC link.exe
    std::string linkCmd = "link " + objFile + " src\\runtime.obj /OUT:" + outFile + " /SUBSYSTEM:CONSOLE";
    if (runCommand(linkCmd) != 0) {
        std::cerr << "Error: linking failed.\n";
        return 1;
    }

    std::cout << "✅ Built executable: " << outFile << "\n";
    return 0;
}
