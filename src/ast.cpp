#include "ast.hpp"
#include <iostream>

// ===== Base AST Classes =====

ExprAST::~ExprAST() {}
StmtAST::~StmtAST() {}

// ===== Expression AST =====

NumberExprAST::NumberExprAST(int v) : value(v) {}
void NumberExprAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Number(" << value << ")\n";
}

StringExprAST::StringExprAST(const std::string &s) : value(s) {}
void StringExprAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "String(\"" << value << "\")\n";
}

VarExprAST::VarExprAST(const std::string &n) : name(n) {}
void VarExprAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Var(" << name << ")\n";
}

UnaryExprAST::UnaryExprAST(const std::string &o, ExprAST *e)
    : op(o), expr(e) {}
void UnaryExprAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Unary(" << op << ")\n";
    expr->print(indent + 2);
}

BinaryExprAST::BinaryExprAST(const std::string &o, ExprAST *l, ExprAST *r)
    : op(o), lhs(l), rhs(r) {}
void BinaryExprAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Binary(" << op << ")\n";
    lhs->print(indent + 2);
    rhs->print(indent + 2);
}

CallExprAST::CallExprAST(const std::string &c, const std::vector<ExprAST*> &a)
    : callee(c), args(a) {}
void CallExprAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Call(" << callee << ")\n";
    for (auto *arg : args) arg->print(indent + 2);
}

// ===== Statement AST =====

ExprStmtAST::ExprStmtAST(ExprAST *e) : expr(e) {}
void ExprStmtAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ExprStmt\n";
    expr->print(indent + 2);
}

VarDeclAST::VarDeclAST(const std::string &n, ExprAST *i)
    : name(n), init(i) {}
void VarDeclAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "VarDecl(" << name << ")\n";
    if (init) init->print(indent + 2);
}

IfStmtAST::IfStmtAST(ExprAST *c,
                     const std::vector<StmtAST*> &t,
                     const std::vector<StmtAST*> &e)
    : cond(c), thenBody(t), elseBody(e) {}
void IfStmtAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "If\n";
    cond->print(indent + 2);
    std::cout << std::string(indent, ' ') << "Then:\n";
    for (auto *s : thenBody) s->print(indent + 2);
    if (!elseBody.empty()) {
        std::cout << std::string(indent, ' ') << "Else:\n";
        for (auto *s : elseBody) s->print(indent + 2);
    }
}

ForStmtAST::ForStmtAST(const std::string &v, ExprAST *s, ExprAST *e,
                       const std::vector<StmtAST*> &b)
    : var(v), start(s), end(e), body(b) {}
void ForStmtAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "For(" << var << ")\n";
    start->print(indent + 2);
    end->print(indent + 2);
    for (auto *s : body) s->print(indent + 2);
}

WhileStmtAST::WhileStmtAST(ExprAST *c, const std::vector<StmtAST*> &b)
    : cond(c), body(b) {}
void WhileStmtAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "While\n";
    cond->print(indent + 2);
    for (auto *s : body) s->print(indent + 2);
}

PrintStmtAST::PrintStmtAST(ExprAST *e) : expr(e) {}
void PrintStmtAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Print\n";
    expr->print(indent + 2);
}

ReturnStmtAST::ReturnStmtAST(ExprAST *e) : expr(e) {}
void ReturnStmtAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Return\n";
    expr->print(indent + 2);
}

FuncDeclAST::FuncDeclAST(const std::string &n,
                         const std::vector<std::string> &p,
                         const std::vector<StmtAST*> &b)
    : name(n), params(p), body(b) {}
void FuncDeclAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Func(" << name << ")\n";
    for (auto *s : body) s->print(indent + 2);
}

ClassDeclAST::ClassDeclAST(const std::string &n, const std::string &b,
                           const std::vector<StmtAST*> &bd)
    : name(n), base(b), body(bd) {}
void ClassDeclAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Class(" << name << " : " << base << ")\n";
    for (auto *s : body) s->print(indent + 2);
}

CaseAST::CaseAST(ExprAST *p, const std::vector<StmtAST*> &b)
    : pattern(p), body(b) {}
void CaseAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Case\n";
    pattern->print(indent + 2);
    for (auto *s : body) s->print(indent + 2);
}

MatchStmtAST::MatchStmtAST(ExprAST *e, const std::vector<CaseAST*> &c)
    : expr(e), cases(c) {}
void MatchStmtAST::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Match\n";
    expr->print(indent + 2);
    for (auto *c : cases) c->print(indent + 2);
}

// ===== Program Root =====

ProgramAST::ProgramAST() {}
void ProgramAST::print() const {
    std::cout << "Program:\n";
    for (auto *s : statements) s->print(2);
}
