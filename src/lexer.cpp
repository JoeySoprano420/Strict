#include "lexer.hpp"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string &src) : source(src), pos(0) {}

char Lexer::peek() const {
    if (pos >= source.size()) return '\0';
    return source[pos];
}

char Lexer::get() {
    if (pos >= source.size()) return '\0';
    return source[pos++];
}

void Lexer::skipWhitespace() {
    while (std::isspace(peek())) get();
}

Token Lexer::nextToken() {
    skipWhitespace();

    char c = peek();
    if (c == '\0') return {TOK_EOF, "", 0};

    // Identifiers & Keywords
    if (std::isalpha(c) || c == '_') {
        std::string ident;
        while (std::isalnum(peek()) || peek() == '_')
            ident.push_back(get());

        // Keywords
        if (ident == "Let") return {TOK_LET, ident};
        if (ident == "If") return {TOK_IF, ident};
        if (ident == "Else") return {TOK_ELSE, ident};
        if (ident == "End") return {TOK_END, ident};
        if (ident == "For") return {TOK_FOR, ident};
        if (ident == "While") return {TOK_WHILE, ident};
        if (ident == "Func") return {TOK_FUNC, ident};
        if (ident == "Return") return {TOK_RETURN, ident};
        if (ident == "Match") return {TOK_MATCH, ident};
        if (ident == "Case") return {TOK_CASE, ident};
        if (ident == "Print") return {TOK_PRINT, ident};
        if (ident == "Input") return {TOK_INPUT, ident};
        if (ident == "Class") return {TOK_CLASS, ident};
        if (ident == "Module") return {TOK_MODULE, ident};
        if (ident == "Import") return {TOK_IMPORT, ident};
        if (ident == "Macro") return {TOK_MACRO, ident};
        if (ident == "Future") return {TOK_FUTURE, ident};
        if (ident == "Parallel") return {TOK_PARALLEL, ident};
        if (ident == "Try") return {TOK_TRY, ident};
        if (ident == "Catch") return {TOK_CATCH, ident};
        if (ident == "Assert") return {TOK_ASSERT, ident};
        if (ident == "Defer") return {TOK_DEFER, ident};
        if (ident == "Interface") return {TOK_INTERFACE, ident};

        return {TOK_IDENTIFIER, ident};
    }

    // Numbers
    if (std::isdigit(c)) {
        std::string num;
        while (std::isdigit(peek())) num.push_back(get());
        return {TOK_NUMBER, num, std::stoi(num)};
    }

    // Strings
    if (c == '"') {
        get(); // consume "
        std::string str;
        while (peek() != '"' && peek() != '\0') {
            str.push_back(get());
        }
        if (peek() == '"') get(); // consume closing "
        return {TOK_STRING, str};
    }

    // Operators and punctuation
    switch (c) {
        case '+': get(); return {TOK_OP, "+"};
        case '-': get(); return {TOK_OP, "-"};
        case '*': get(); return {TOK_OP, "*"};
        case '/': get(); return {TOK_OP, "/"};
        case '=':
            get();
            if (peek() == '=') { get(); return {TOK_OP, "=="}; }
            return {TOK_ASSIGN, "="};
        case '<':
            get();
            if (peek() == '=') { get(); return {TOK_OP, "<="}; }
            return {TOK_OP, "<"};
        case '>':
            get();
            if (peek() == '=') { get(); return {TOK_OP, ">="}; }
            return {TOK_OP, ">"};
        case '(': get(); return {TOK_LPAREN, "("};
        case ')': get(); return {TOK_RPAREN, ")"};
        case '{': get(); return {TOK_LBRACE, "{"};
        case '}': get(); return {TOK_RBRACE, "}"};
        case '[': get(); return {TOK_LBRACK, "["};
        case ']': get(); return {TOK_RBRACK, "]"};
        case ',': get(); return {TOK_COMMA, ","};
        case ':': get(); return {TOK_COLON, ":"};
        case '.':
            get();
            if (peek() == '.') { get(); return {TOK_DOTDOT, ".."}; }
            return {TOK_DOT, "."};
    }

    throw std::runtime_error(std::string("Unexpected character: ") + c);
}
