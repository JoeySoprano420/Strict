#pragma once
#include "lexer.hpp"
#include "ast.hpp"
#include <vector>
#include <string>

// === Parser ===
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

    // Statements
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

    // Helpers
    std::vector<StmtAST*> parseBlock();

    // Expressions
    ExprAST* parseExpression();
    ExprAST* parseEquality();
    ExprAST* parseComparison();
    ExprAST* parseTerm();
    ExprAST* parseFactor();
    ExprAST* parseUnary();
    ExprAST* parsePrimary();
};
