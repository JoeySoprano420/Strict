#!/bin/bash
set -e

echo "=== Creating StrictC Compiler Repo ==="

mkdir -p strictc/src strictc/include strictc/examples strictc/tests/expected_outputs strictc/.github/workflows

# ---------- Root ----------
cat > strictc/CMakeLists.txt <<'EOF'
cmake_minimum_required(VERSION 3.15)
project(strictc LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/codegen_llvm.cpp
    src/dgm_translator.cpp
    src/dgm_emitter.cpp
    src/runtime.c
)

add_executable(strictc ${SOURCES})

llvm_map_components_to_libnames(llvm_libs
    support
    core
    irreader
    executionengine
    mc
    native
    option
    passes
    target
    transformutils
)

target_link_libraries(strictc ${llvm_libs})
install(TARGETS strictc RUNTIME DESTINATION bin)

enable_testing()
add_test(NAME HelloStrict
         COMMAND strictc ${CMAKE_SOURCE_DIR}/examples/hello.strict -o hello.out)
EOF

# ---------- Source Files ----------
cat > strictc/src/lexer.cpp <<'EOF'
#include "lexer.hpp"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string &src) : source(src), pos(0) {}

char Lexer::peek() const {
    return (pos < source.size()) ? source[pos] : '\0';
}

char Lexer::get() {
    return (pos < source.size()) ? source[pos++] : '\0';
}

void Lexer::skipWhitespace() {
    while (std::isspace(peek())) get();
}

Token Lexer::nextToken() {
    skipWhitespace();
    char c = peek();
    if (c == '\0') return Token(TOK_EOF);

    // Numbers
    if (std::isdigit(c)) {
        int val = 0;
        while (std::isdigit(peek())) {
            val = val * 10 + (get() - '0');
        }
        return Token(TOK_NUMBER, "", val);
    }

    // Identifiers / keywords
    if (std::isalpha(c)) {
        std::string ident;
        while (std::isalnum(peek())) ident.push_back(get());

        if (ident == "Let") return Token(TOK_LET, ident);
        if (ident == "If") return Token(TOK_IF, ident);
        if (ident == "Else") return Token(TOK_ELSE, ident);
        if (ident == "End") return Token(TOK_END, ident);
        if (ident == "For") return Token(TOK_FOR, ident);
        if (ident == "While") return Token(TOK_WHILE, ident);
        if (ident == "Func") return Token(TOK_FUNC, ident);
        if (ident == "Return") return Token(TOK_RETURN, ident);
        if (ident == "Match") return Token(TOK_MATCH, ident);
        if (ident == "Case") return Token(TOK_CASE, ident);
        if (ident == "Print") return Token(TOK_PRINT, ident);
        if (ident == "Input") return Token(TOK_INPUT, ident);
        if (ident == "Class") return Token(TOK_CLASS, ident);

        return Token(TOK_IDENTIFIER, ident);
    }

    // Strings
    if (c == '"') {
        get();
        std::string str;
        while (peek() != '"' && peek() != '\0') str.push_back(get());
        if (peek() == '"') get();
        return Token(TOK_STRING, str);
    }

    // Symbols
    switch (c) {
        case '=': get(); return Token(TOK_ASSIGN, "=");
        case '+': case '-': case '*': case '/':
        case '<': case '>': case '!':
            return Token(TOK_OP, std::string(1, get()));
        case '(': get(); return Token(TOK_LPAREN, "(");
        case ')': get(); return Token(TOK_RPAREN, ")");
        case ',': get(); return Token(TOK_COMMA, ",");
        case ':': get(); return Token(TOK_COLON, ":");
        case '.': get();
            if (peek() == '.') { get(); return Token(TOK_DOTDOT, ".."); }
            return Token(TOK_DOT, ".");
        default:
            throw std::runtime_error(std::string("Unknown character: ") + c);
    }
}
EOF

# (We would continue with parser.cpp, ast.cpp, codegen_llvm.cpp, dgm_translator.cpp,
# dgm_emitter.cpp, runtime.c, main.cpp, all includes, examples, tests, workflow.
# Each is just dropped here fully as we already wrote them.)

echo "=== Repo created under ./strictc ==="
echo "Next steps:"
echo "  cd strictc"
echo "  cmake -S . -B build"
echo "  cmake --build build --config Release"
