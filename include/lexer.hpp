#pragma once
#include <string>
#include <vector>

// === Token Types ===
enum TokenType {
    // Special
    TOK_EOF,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_STRING,

    // Keywords
    TOK_LET,
    TOK_IF,
    TOK_ELSE,
    TOK_END,
    TOK_FOR,
    TOK_WHILE,
    TOK_FUNC,
    TOK_RETURN,
    TOK_MATCH,
    TOK_CASE,
    TOK_PRINT,
    TOK_INPUT,
    TOK_CLASS,
    TOK_MODULE,
    TOK_IMPORT,
    TOK_MACRO,
    TOK_FUTURE,
    TOK_PARALLEL,
    TOK_TRY,
    TOK_CATCH,
    TOK_ASSERT,
    TOK_DEFER,
    TOK_INTERFACE,

    // Operators & symbols
    TOK_OP,
    TOK_ASSIGN,   // "="
    TOK_LPAREN,   // "("
    TOK_RPAREN,   // ")"
    TOK_LBRACE,   // "{"
    TOK_RBRACE,   // "}"
    TOK_LBRACK,   // "["
    TOK_RBRACK,   // "]"
    TOK_COMMA,    // ","
    TOK_COLON,    // ":"
    TOK_DOT,
    TOK_DOTDOT    // ".."
};

// === Token Struct ===
struct Token {
    TokenType type;
    std::string text;
    int intVal;

    Token(TokenType t = TOK_EOF, const std::string &s = "", int v = 0)
        : type(t), text(s), intVal(v) {}
};

// === Lexer ===
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
