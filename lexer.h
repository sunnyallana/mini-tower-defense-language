#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>
#include "token.h"

// Lexical Analyzer - converts source code into tokens
class Lexer {
public:
    Lexer(const std::string& source);

    Token getNextToken();    // Get next token and advance
    Token peekToken();       // Look at next token without advancing

private:
    std::string source;                         // Source code to analyze
    size_t position;                           // Current reading position
    int currentLine;                           // Current line number
    std::unordered_map<std::string, TokenType> keywords;  // Keyword lookup table

    char peek();                // Look at next character without consuming
    char advance();             // Consume and return next character
    bool isAtEnd();             // Check if reached end of source

    Token identifier();         // Process identifier or keyword
    Token number();             // Process integer or float literal
    void skipWhitespace();      // Skip spaces, tabs, newlines
    void skipComment();         // Skip single-line comments
};

#endif
