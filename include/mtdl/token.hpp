#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

// All possible token types in the MTDL language
enum class TokenType {
    // Keywords
    MAP, ENEMY, TOWER, WAVE, SPAWN, PLACE, AT,
    SIZE, PATH, COUNT, START, INTERVAL,

    // Identifiers and literals
    IDENT, INT, FLOAT,

    // Punctuation and operators
    LBRACE, RBRACE, LPAREN, RPAREN, LBRACKET, RBRACKET,
    COMMA, SEMICOLON, EQUAL,
    PLUS, MINUS, MUL, DIV,  // Future arithmetic support

    // Special tokens
    END_OF_FILE,
    UNKNOWN
};

// Represents a single token with its type, text, and position
struct Token {
    TokenType type;      // Token classification
    std::string lexeme;  // Actual text from source
    int line;           // Line number in source file

    Token() : type(TokenType::UNKNOWN), lexeme(""), line(0) {}

    Token(TokenType t, const std::string& lx, int ln)
        : type(t), lexeme(lx), line(ln) {}
};

#endif
