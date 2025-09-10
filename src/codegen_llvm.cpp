#include "codegen.hpp"
#include "ast.hpp"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>

using namespace llvm;

static LLVMContext TheContext;
static IRBuilder<> Builder(TheContext);
static std::unique_ptr<Module> TheModule;
static std::map<std::string, Value*> NamedValues;
static std::map<std::string, Function*> FunctionTable;

// === Helpers ===

Value* logError(const std::string &msg) {
    errs() << "Codegen error: " << msg << "\n";
    return nullptr;
}

// === Expr Codegen ===

Value* NumberExprAST::codegen() {
    return ConstantInt::get(Type::getInt32Ty(TheContext), value);
}

Value* StringExprAST::codegen() {
    return Builder.CreateGlobalStringPtr(value, "str");
}

Value* VarExprAST::codegen() {
    if (NamedValues.count(name))
        return NamedValues[name];
    return logError("Unknown variable: " + name);
}

Value* UnaryExprAST::codegen() {
    Value* val = expr->codegen();
    if (!val) return nullptr;

    if (op == "-")
        return Builder.CreateNeg(val, "negtmp");
    if (op == "!")
        return Builder.CreateNot(val, "nottmp");

    return logError("Invalid unary operator: " + op);
}

Value* BinaryExprAST::codegen() {
    Value* L = lhs->codegen();
    Value* R = rhs->codegen();
    if (!L || !R) return nullptr;

    if (op == "+") return Builder.CreateAdd(L, R, "addtmp");
    if (op == "-") return Builder.CreateSub(L, R, "subtmp");
    if (op == "*") return Builder.CreateMul(L, R, "multmp");
    if (op == "/") return Builder.CreateSDiv(L, R, "divtmp");

    if (op == "<") {
        Value* cmp = Builder.CreateICmpSLT(L, R, "cmptmp");
        return Builder.CreateZExt(cmp, Type::getInt32Ty(TheContext), "booltmp");
    }
    if (op == ">") {
        Value* cmp = Builder.CreateICmpSGT(L, R, "cmptmp");
        return Builder.CreateZExt(cmp, Type::getInt32Ty(TheContext), "booltmp");
    }
    if (op == "==") {
        Value* cmp = Builder.CreateICmpEQ(L, R, "cmptmp");
        return Builder.CreateZExt(cmp, Type::getInt32Ty(TheContext), "booltmp");
    }

    return logError("Unknown binary operator: " + op);
}

Value* CallExprAST::codegen() {
    Function* calleeF = TheModule->getFunction(callee);
    if (!calleeF) return logError("Unknown function: " + callee);

    std::vector<Value*> argsV;
    for (auto *arg : args) {
        Value* a = arg->codegen();
        if (!a) return nullptr;
        argsV.push_back(a);
    }

    return Builder.CreateCall(calleeF, argsV, "calltmp");
}

// === Statement Codegen ===

Value* ExprStmtAST::codegen() {
    return expr->codegen();
}

Value* VarDeclAST::codegen() {
    Value* initVal = init ? init->codegen()
                          : ConstantInt::get(Type::getInt32Ty(TheContext), 0);
    AllocaInst* alloc = Builder.CreateAlloca(Type::getInt32Ty(TheContext), 0, name.c_str());
    Builder.CreateStore(initVal, alloc);
    NamedValues[name] = alloc;
    return alloc;
}

Value* IfStmtAST::codegen() {
    Value* condV = cond->codegen();
    if (!condV) return nullptr;

    condV = Builder.CreateICmpNE(condV,
                                 ConstantInt::get(Type::getInt32Ty(TheContext), 0),
                                 "ifcond");

    Function* parentF = Builder.GetInsertBlock()->getParent();

    BasicBlock* thenBB = BasicBlock::Create(TheContext, "then", parentF);
    BasicBlock* elseBB = BasicBlock::Create(TheContext, "else");
    BasicBlock* mergeBB = BasicBlock::Create(TheContext, "ifcont");

    Builder.CreateCondBr(condV, thenBB, elseBB);

    // Then
    Builder.SetInsertPoint(thenBB);
    for (auto *s : thenBody) s->codegen();
    Builder.CreateBr(mergeBB);
    thenBB = Builder.GetInsertBlock();

    // Else
    parentF->getBasicBlockList().push_back(elseBB);
    Builder.SetInsertPoint(elseBB);
    for (auto *s : elseBody) s->codegen();
    Builder.CreateBr(mergeBB);
    elseBB = Builder.GetInsertBlock();

    // Merge
    parentF->getBasicBlockList().push_back(mergeBB);
    Builder.SetInsertPoint(mergeBB);

    return nullptr;
}

Value* PrintStmtAST::codegen() {
    Function* printFn = TheModule->getFunction("strict_print");
    if (!printFn) {
        FunctionType* FT = FunctionType::get(Type::getVoidTy(TheContext),
                                             {Type::getInt8PtrTy(TheContext)}, false);
        printFn = Function::Create(FT, Function::ExternalLinkage,
                                   "strict_print", TheModule.get());
    }
    Value* strVal = expr->codegen();
    return Builder.CreateCall(printFn, {strVal});
}

Value* ReturnStmtAST::codegen() {
    Value* val = expr->codegen();
    return Builder.CreateRet(val);
}

Value* FuncDeclAST::codegen() {
    std::vector<Type*> paramTypes(params.size(), Type::getInt32Ty(TheContext));
    FunctionType* FT = FunctionType::get(Type::getInt32Ty(TheContext),
                                         paramTypes, false);
    Function* F = Function::Create(FT, Function::ExternalLinkage,
                                   name, TheModule.get());
    FunctionTable[name] = F;

    BasicBlock* BB = BasicBlock::Create(TheContext, "entry", F);
    Builder.SetInsertPoint(BB);

    unsigned idx = 0;
    for (auto &arg : F->args()) {
        AllocaInst* alloc = Builder.CreateAlloca(Type::getInt32Ty(TheContext), 0, params[idx].c_str());
        Builder.CreateStore(&arg, alloc);
        NamedValues[params[idx]] = alloc;
        idx++;
    }

    for (auto *s : body) s->codegen();

    verifyFunction(*F);
    return F;
}

Value* ProgramAST::codegen() {
    TheModule = std::make_unique<Module>("strict", TheContext);

    // Prototype for runtime input
    FunctionType* inFT = FunctionType::get(Type::getInt32Ty(TheContext), false);
    Function::Create(inFT, Function::ExternalLinkage,
                     "strict_input", TheModule.get());

    // Prototype for runtime print
    FunctionType* printFT = FunctionType::get(Type::getVoidTy(TheContext),
                                              {Type::getInt8PtrTy(TheContext)}, false);
    Function::Create(printFT, Function::ExternalLinkage,
                     "strict_print", TheModule.get());

    // Generate program body
    for (auto *s : statements) {
        s->codegen();
    }

    return nullptr;
}

// === Emit LLVM IR to file ===

void ProgramAST::emitIR(const std::string &filename) {
    std::error_code EC;
    raw_fd_ostream dest(filename, EC, sys::fs::OF_None);
    if (EC) {
        errs() << "Could not open file: " << EC.message();
        return;
    }
    TheModule->print(dest, nullptr);
}
