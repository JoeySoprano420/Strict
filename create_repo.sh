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

# ---------- Include ----------
cat > strictc/include/lexer.hpp <<'EOF'
#pragma once
#include <string>
#include <vector>

enum TokenType {
    TOK_EOF, TOK_IDENTIFIER, TOK_NUMBER, TOK_STRING,
    TOK_LET, TOK_IF, TOK_ELSE, TOK_END, TOK_FOR, TOK_WHILE,
    TOK_FUNC, TOK_RETURN, TOK_MATCH, TOK_CASE, TOK_PRINT, TOK_INPUT,
    TOK_CLASS, TOK_MODULE, TOK_IMPORT, TOK_MACRO, TOK_FUTURE, TOK_PARALLEL,
    TOK_TRY, TOK_CATCH, TOK_ASSERT, TOK_DEFER, TOK_INTERFACE,
    TOK_OP, TOK_ASSIGN, TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE, TOK_LBRACK, TOK_RBRACK,
    TOK_COMMA, TOK_COLON, TOK_DOT, TOK_DOTDOT
};

struct Token {
    TokenType type;
    std::string text;
    int intVal;
    Token(TokenType t = TOK_EOF, const std::string &s = "", int v = 0)
        : type(t), text(s), intVal(v) {}
};

class Lexer {
    std::string source;
    size_t pos;
public:
    Lexer(const std::string &src);
    Token nextToken();
private:
    char peek() const;
    char get();
    void skipWhitespace();
};
EOF

cat > strictc/include/parser.hpp <<'EOF'
#pragma once
#include "lexer.hpp"
#include "ast.hpp"
#include <vector>
#include <string>

class Parser {
    Lexer &lexer;
    Token current;
public:
    Parser(Lexer &lex);
    ProgramAST parseProgram();
private:
    void advance();
    bool match(TokenType type);
    void expect(TokenType type, const std::string &err);
    StmtAST* parseStatement();
    StmtAST* parseVarDecl();
    StmtAST* parseFunc();
    StmtAST* parseClass();
    StmtAST* parseIf();
    StmtAST* parseFor();
    StmtAST* parseWhile();
    StmtAST* parseMatch();
    StmtAST* parsePrint();
    StmtAST* parseReturn();
    StmtAST* parseExprStmt();
    std::vector<StmtAST*> parseBlock();
    ExprAST* parseExpression();
    ExprAST* parseEquality();
    ExprAST* parseComparison();
    ExprAST* parseTerm();
    ExprAST* parseFactor();
    ExprAST* parseUnary();
    ExprAST* parsePrimary();
};
EOF

cat > strictc/include/ast.hpp <<'EOF'
#pragma once
#include <string>
#include <vector>
#include <map>
#include <llvm/IR/Value.h>

struct ExprAST { virtual ~ExprAST(); virtual void print(int indent=0) const=0; virtual llvm::Value* codegen()=0; };
struct StmtAST { virtual ~StmtAST(); virtual void print(int indent=0) const=0; virtual llvm::Value* codegen()=0; };

struct NumberExprAST : ExprAST { int value; NumberExprAST(int v); void print(int i) const override; llvm::Value* codegen() override; };
struct StringExprAST : ExprAST { std::string value; StringExprAST(const std::string &s); void print(int i) const override; llvm::Value* codegen() override; };
struct VarExprAST : ExprAST { std::string name; VarExprAST(const std::string &n); void print(int i) const override; llvm::Value* codegen() override; };
struct UnaryExprAST : ExprAST { std::string op; ExprAST* expr; UnaryExprAST(const std::string&o,ExprAST*e); void print(int i) const override; llvm::Value* codegen() override; };
struct BinaryExprAST : ExprAST { std::string op; ExprAST* lhs; ExprAST* rhs; BinaryExprAST(const std::string&o,ExprAST*l,ExprAST*r); void print(int i) const override; llvm::Value* codegen() override; };
struct CallExprAST : ExprAST { std::string callee; std::vector<ExprAST*> args; CallExprAST(const std::string&c,const std::vector<ExprAST*>&a); void print(int i) const override; llvm::Value* codegen() override; };

struct ExprStmtAST:StmtAST{ExprAST*expr;ExprStmtAST(ExprAST*e);void print(int i) const override;llvm::Value*codegen() override;};
struct VarDeclAST:StmtAST{std::string name;ExprAST*init;VarDeclAST(const std::string&n,ExprAST*i);void print(int i) const override;llvm::Value*codegen() override;};
struct IfStmtAST:StmtAST{ExprAST*cond;std::vector<StmtAST*>thenBody,elseBody;IfStmtAST(ExprAST*c,const std::vector<StmtAST*>&t,const std::vector<StmtAST*>&e);void print(int i) const override;llvm::Value*codegen() override;};
struct ForStmtAST:StmtAST{std::string var;ExprAST*start;ExprAST*end;std::vector<StmtAST*>body;ForStmtAST(const std::string&v,ExprAST*s,ExprAST*e,const std::vector<StmtAST*>&b);void print(int i) const override;llvm::Value*codegen() override;};
struct WhileStmtAST:StmtAST{ExprAST*cond;std::vector<StmtAST*>body;WhileStmtAST(ExprAST*c,const std::vector<StmtAST*>&b);void print(int i) const override;llvm::Value*codegen() override;};
struct PrintStmtAST:StmtAST{ExprAST*expr;PrintStmtAST(ExprAST*e);void print(int i) const override;llvm::Value*codegen() override;};
struct ReturnStmtAST:StmtAST{ExprAST*expr;ReturnStmtAST(ExprAST*e);void print(int i) const override;llvm::Value*codegen() override;};
struct FuncDeclAST:StmtAST{std::string name;std::vector<std::string>params;std::vector<StmtAST*>body;FuncDeclAST(const std::string&n,const std::vector<std::string>&p,const std::vector<StmtAST*>&b);void print(int i) const override;llvm::Value*codegen() override;};
struct ClassDeclAST:StmtAST{std::string name,base;std::vector<StmtAST*>body;ClassDeclAST(const std::string&n,const std::string&b,const std::vector<StmtAST*>&bd);void print(int i) const override;llvm::Value*codegen() override;};
struct CaseAST{ExprAST*pattern;std::vector<StmtAST*>body;CaseAST(ExprAST*p,const std::vector<StmtAST*>&b);void print(int i) const;};
struct MatchStmtAST:StmtAST{ExprAST*expr;std::vector<CaseAST*>cases;MatchStmtAST(ExprAST*e,const std::vector<CaseAST*>&c);void print(int i) const override;llvm::Value*codegen() override;};

struct ProgramAST {
    std::vector<StmtAST*> statements;
    ProgramAST(); void print() const;
    llvm::Value* codegen(); void emitIR(const std::string&filename);
    llvm::Module* getModule();
};
EOF

cat > strictc/include/codegen.hpp <<'EOF'
#pragma once
#include <string>
#include <llvm/IR/Value.h>
struct ProgramAST;
llvm::Value* codegenProgram(ProgramAST &program);
void emitIR(ProgramAST &program,const std::string &filename);
EOF

cat > strictc/include/dgm.hpp <<'EOF'
#pragma once
#include <string>
#include <llvm/IR/Module.h>
void translateModuleToDGM(llvm::Module &M,const std::string &filename);
void emitDGMtoNASM(const std::string &dgmFile,const std::string &nasmFile);
EOF

# ---------- Examples ----------
cat > strictc/examples/hello.strict <<'EOF'
Func Square(n)
    Return n * n
End

Print "Enter a number:"
Let x = Input

If x < 0 Then
    Print "Negative number"
Else
    Print "Square is:"
    Print Call Square(x)
End

For i = 1..5 Then
    Print "Loop iteration:"
    Print i
End
EOF

cat > strictc/examples/math.strict <<'EOF'
Func AddAndDivide(a, b, c)
    Let sum = Safe.Add a, b
    Return Safe.Div sum, c
End

Print "Enter three numbers (a, b, c):"
Let a = Input
Let b = Input
Let c = Input

Let result = Call AddAndDivide(a, b, c)

Match result
    Case 0: Print "Result is zero"
    Case 1..5: Print "Result is small"
    Case 6..100: Print "Result is medium"
    Case <0: Print "Error or negative result"
    Case *: Print "Result is large"
End
EOF

# (Add match.strict, oop.strict, generics.strict, tests/run_tests.sh, expected_outputs/*, workflow etc similarly)

echo "=== Repo created under ./strictc ==="
echo "Next steps:"
echo "  cd strictc"
echo "  cmake -S . -B build"
echo "  cmake --build build --config Release"
