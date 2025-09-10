#include "parser.hpp"
#include <stdexcept>
#include <iostream>

Parser::Parser(Lexer &lex) : lexer(lex) {
    advance();
}

void Parser::advance() {
    current = lexer.nextToken();
}

bool Parser::match(TokenType type) {
    if (current.type == type) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const std::string &err) {
    if (!match(type)) {
        throw std::runtime_error("Parse error: expected " + err + " but got " + current.text);
    }
}

ProgramAST Parser::parseProgram() {
    ProgramAST program;
    while (current.type != TOK_EOF) {
        program.statements.push_back(parseStatement());
    }
    return program;
}

StmtAST* Parser::parseStatement() {
    switch (current.type) {
        case TOK_LET: return parseVarDecl();
        case TOK_IF: return parseIf();
        case TOK_FOR: return parseFor();
        case TOK_WHILE: return parseWhile();
        case TOK_FUNC: return parseFunc();
        case TOK_CLASS: return parseClass();
        case TOK_MATCH: return parseMatch();
        case TOK_PRINT: return parsePrint();
        case TOK_RETURN: return parseReturn();
        default: return parseExprStmt();
    }
}

// --- Declarations ---

StmtAST* Parser::parseVarDecl() {
    advance(); // consume Let
    std::string name = current.text;
    expect(TOK_IDENTIFIER, "identifier");

    ExprAST* init = nullptr;
    if (match(TOK_ASSIGN)) {
        init = parseExpression();
    }
    return new VarDeclAST(name, init);
}

StmtAST* Parser::parseFunc() {
    advance(); // consume Func
    std::string name = current.text;
    expect(TOK_IDENTIFIER, "function name");

    expect(TOK_LPAREN, "(");
    std::vector<std::string> params;
    if (current.type != TOK_RPAREN) {
        do {
            params.push_back(current.text);
            expect(TOK_IDENTIFIER, "parameter name");
        } while (match(TOK_COMMA));
    }
    expect(TOK_RPAREN, ")");

    auto body = parseBlock();
    return new FuncDeclAST(name, params, body);
}

StmtAST* Parser::parseClass() {
    advance(); // consume Class
    std::string name = current.text;
    expect(TOK_IDENTIFIER, "class name");

    std::string base;
    if (match(TOK_COLON)) {
        base = current.text;
        expect(TOK_IDENTIFIER, "base class name");
    }

    auto body = parseBlock();
    return new ClassDeclAST(name, base, body);
}

// --- Control Flow ---

StmtAST* Parser::parseIf() {
    advance(); // consume If
    ExprAST* cond = parseExpression();
    auto thenBody = parseBlock();

    std::vector<StmtAST*> elseBody;
    if (match(TOK_ELSE)) {
        elseBody = parseBlock();
    }
    return new IfStmtAST(cond, thenBody, elseBody);
}

StmtAST* Parser::parseFor() {
    advance(); // consume For
    std::string var = current.text;
    expect(TOK_IDENTIFIER, "loop variable");
    expect(TOK_ASSIGN, "=");

    ExprAST* start = parseExpression();
    expect(TOK_DOTDOT, "..");
    ExprAST* end = parseExpression();

    auto body = parseBlock();
    return new ForStmtAST(var, start, end, body);
}

StmtAST* Parser::parseWhile() {
    advance(); // consume While
    ExprAST* cond = parseExpression();
    auto body = parseBlock();
    return new WhileStmtAST(cond, body);
}

StmtAST* Parser::parseMatch() {
    advance(); // consume Match
    ExprAST* expr = parseExpression();

    std::vector<CaseAST*> cases;
    while (current.type == TOK_CASE) {
        advance(); // consume Case
        ExprAST* pattern = parseExpression();
        expect(TOK_COLON, ":");
        auto body = parseBlock();
        cases.push_back(new CaseAST(pattern, body));
    }

    expect(TOK_END, "End");
    advance();
    return new MatchStmtAST(expr, cases);
}

// --- Simple Statements ---

StmtAST* Parser::parsePrint() {
    advance(); // consume Print
    ExprAST* expr = parseExpression();
    return new PrintStmtAST(expr);
}

StmtAST* Parser::parseReturn() {
    advance(); // consume Return
    ExprAST* expr = parseExpression();
    return new ReturnStmtAST(expr);
}

StmtAST* Parser::parseExprStmt() {
    ExprAST* expr = parseExpression();
    return new ExprStmtAST(expr);
}

// --- Block Helper ---

std::vector<StmtAST*> Parser::parseBlock() {
    std::vector<StmtAST*> stmts;
    while (current.type != TOK_END && current.type != TOK_ELSE && current.type != TOK_CASE && current.type != TOK_EOF) {
        stmts.push_back(parseStatement());
    }
    if (current.type == TOK_END) {
        advance();
    }
    return stmts;
}

// --- Expressions ---

ExprAST* Parser::parseExpression() {
    return parseEquality();
}

ExprAST* Parser::parseEquality() {
    ExprAST* left = parseComparison();
    while (current.type == TOK_OP && (current.text == "==" || current.text == "!=")) {
        std::string op = current.text;
        advance();
        ExprAST* right = parseComparison();
        left = new BinaryExprAST(op, left, right);
    }
    return left;
}

ExprAST* Parser::parseComparison() {
    ExprAST* left = parseTerm();
    while (current.type == TOK_OP &&
           (current.text == "<" || current.text == "<=" ||
            current.text == ">" || current.text == ">=")) {
        std::string op = current.text;
        advance();
        ExprAST* right = parseTerm();
        left = new BinaryExprAST(op, left, right);
    }
    return left;
}

ExprAST* Parser::parseTerm() {
    ExprAST* left = parseFactor();
    while (current.type == TOK_OP && (current.text == "+" || current.text == "-")) {
        std::string op = current.text;
        advance();
        ExprAST* right = parseFactor();
        left = new BinaryExprAST(op, left, right);
    }
    return left;
}

ExprAST* Parser::parseFactor() {
    ExprAST* left = parseUnary();
    while (current.type == TOK_OP && (current.text == "*" || current.text == "/")) {
        std::string op = current.text;
        advance();
        ExprAST* right = parseUnary();
        left = new BinaryExprAST(op, left, right);
    }
    return left;
}

ExprAST* Parser::parseUnary() {
    if (current.type == TOK_OP && (current.text == "-" || current.text == "!")) {
        std::string op = current.text;
        advance();
        ExprAST* operand = parseUnary();
        return new UnaryExprAST(op, operand);
    }
    return parsePrimary();
}

ExprAST* Parser::parsePrimary() {
    if (current.type == TOK_NUMBER) {
        int val = current.intVal;
        advance();
        return new NumberExprAST(val);
    }
    if (current.type == TOK_STRING) {
        std::string str = current.text;
        advance();
        return new StringExprAST(str);
    }
    if (current.type == TOK_IDENTIFIER) {
        std::string name = current.text;
        advance();
        // Function call?
        if (match(TOK_LPAREN)) {
            std::vector<ExprAST*> args;
            if (current.type != TOK_RPAREN) {
                do {
                    args.push_back(parseExpression());
                } while (match(TOK_COMMA));
            }
            expect(TOK_RPAREN, ")");
            return new CallExprAST(name, args);
        }
        return new VarExprAST(name);
    }
    if (match(TOK_LPAREN)) {
        ExprAST* expr = parseExpression();
        expect(TOK_RPAREN, ")");
        return expr;
    }
    throw std::runtime_error("Unexpected token in expression: " + current.text);
}
