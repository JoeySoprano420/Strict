#include "dgm.hpp"
#include <fstream>
#include <iostream>
#include <unordered_map>

// === DGM → NASM instruction map ===
// NOTE: This is a small working subset. The full 144 entries
// should be included for production, but we wire the core ones here.
static std::unordered_map<std::string, std::string> NasmMap = {
    {"17 add",  "add rax, rbx"},
    {"18 sub",  "sub rax, rbx"},
    {"19 mul",  "imul rax, rbx"},
    {"1B sdiv", "idiv rbx"},
    {"15 icmp", "cmp rax, rbx"},
    {"30 br",   "jmp"},
    {"2B call", "call"},
    {"33 ret",  "ret"},
    {"01 alloca","sub rsp, 8"},
    {"02 load", "mov rax, [rbx]"},
    {"03 store","mov [rax], rbx"}
};

// === Emit NASM Assembly ===
void emitDGMtoNASM(const std::string &dgmFile, const std::string &nasmFile) {
    std::ifstream in(dgmFile);
    if (!in.is_open()) {
        std::cerr << "Error: cannot open " << dgmFile << "\n";
        return;
    }

    std::ofstream out(nasmFile);
    if (!out.is_open()) {
        std::cerr << "Error: cannot open " << nasmFile << "\n";
        return;
    }

    // Assembly header
    out << "section .text\n";
    out << "global main\n";
    out << "extern strict_print\n";
    out << "extern strict_input\n\n";

    out << "main:\n";

    std::string line;
    while (std::getline(in, line)) {
        // Ignore FUNC/BLOCK markers
        if (line.find("FUNC") != std::string::npos) continue;
        if (line.find("BLOCK") != std::string::npos) continue;
        if (line.find("END FUNC") != std::string::npos) continue;

        // Extract opcode
        size_t pos = line.find(';');
        std::string op = (pos == std::string::npos) ? line : line.substr(0, pos);
        // Trim whitespace
        op.erase(0, op.find_first_not_of(" \t"));
        op.erase(op.find_last_not_of(" \t") + 1);

        if (NasmMap.count(op)) {
            out << "    " << NasmMap[op] << "\n";
        } else {
            out << "    ; unhandled: " << op << "\n";
        }
    }

    // Exit
    out << "    mov rax, 60     ; sys_exit\n";
    out << "    xor rdi, rdi\n";
    out << "    syscall\n";

    in.close();
    out.close();
}
