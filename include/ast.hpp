#pragma once
#include <string>
#include <vector>
#include <map>
#include <llvm/IR/Value.h>

// === Base Classes ===

struct ExprAST {
    virtual ~ExprAST();
    virtual void print(int indent = 0) const = 0;
    virtual llvm::Value* codegen() = 0;
};

struct StmtAST {
    virtual ~StmtAST();
    virtual void print(int indent = 0) const = 0;
    virtual llvm::Value* codegen() = 0;
};

// === Expressions ===

struct NumberExprAST : public ExprAST {
    int value;
    NumberExprAST(int v);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct StringExprAST : public ExprAST {
    std::string value;
    StringExprAST(const std::string &s);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct VarExprAST : public ExprAST {
    std::string name;
    VarExprAST(const std::string &n);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct UnaryExprAST : public ExprAST {
    std::string op;
    ExprAST *expr;
    UnaryExprAST(const std::string &o, ExprAST *e);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct BinaryExprAST : public ExprAST {
    std::string op;
    ExprAST *lhs;
    ExprAST *rhs;
    BinaryExprAST(const std::string &o, ExprAST *l, ExprAST *r);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct CallExprAST : public ExprAST {
    std::string callee;
    std::vector<ExprAST*> args;
    CallExprAST(const std::string &c, const std::vector<ExprAST*> &a);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

// === Statements ===

struct ExprStmtAST : public StmtAST {
    ExprAST *expr;
    ExprStmtAST(ExprAST *e);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct VarDeclAST : public StmtAST {
    std::string name;
    ExprAST *init;
    VarDeclAST(const std::string &n, ExprAST *i);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct IfStmtAST : public StmtAST {
    ExprAST *cond;
    std::vector<StmtAST*> thenBody;
    std::vector<StmtAST*> elseBody;
    IfStmtAST(ExprAST *c, const std::vector<StmtAST*> &t,
              const std::vector<StmtAST*> &e);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct ForStmtAST : public StmtAST {
    std::string var;
    ExprAST *start;
    ExprAST *end;
    std::vector<StmtAST*> body;
    ForStmtAST(const std::string &v, ExprAST *s, ExprAST *e,
               const std::vector<StmtAST*> &b);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct WhileStmtAST : public StmtAST {
    ExprAST *cond;
    std::vector<StmtAST*> body;
    WhileStmtAST(ExprAST *c, const std::vector<StmtAST*> &b);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct PrintStmtAST : public StmtAST {
    ExprAST *expr;
    PrintStmtAST(ExprAST *e);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct ReturnStmtAST : public StmtAST {
    ExprAST *expr;
    ReturnStmtAST(ExprAST *e);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct FuncDeclAST : public StmtAST {
    std::string name;
    std::vector<std::string> params;
    std::vector<StmtAST*> body;
    FuncDeclAST(const std::string &n,
                const std::vector<std::string> &p,
                const std::vector<StmtAST*> &b);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct ClassDeclAST : public StmtAST {
    std::string name;
    std::string base;
    std::vector<StmtAST*> body;
    ClassDeclAST(const std::string &n, const std::string &b,
                 const std::vector<StmtAST*> &bd);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

struct CaseAST {
    ExprAST *pattern;
    std::vector<StmtAST*> body;
    CaseAST(ExprAST *p, const std::vector<StmtAST*> &b);
    void print(int indent) const;
};

struct MatchStmtAST : public StmtAST {
    ExprAST *expr;
    std::vector<CaseAST*> cases;
    MatchStmtAST(ExprAST *e, const std::vector<CaseAST*> &c);
    void print(int indent) const override;
    llvm::Value* codegen() override;
};

// === Program Root ===

struct ProgramAST {
    std::vector<StmtAST*> statements;
    ProgramAST();
    void print() const;
    llvm::Value* codegen();
    void emitIR(const std::string &filename);
    llvm::Module* getModule();
};
