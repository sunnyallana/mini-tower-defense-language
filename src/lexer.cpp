#include "mtdl/lexer.hpp"
#include <iostream>
#include <cctype>

Lexer::Lexer(const std::string& source)
    : source(source), position(0), currentLine(1) {
    // Initialize keyword lookup table
    keywords = {
        {"map", TokenType::MAP},
        {"enemy", TokenType::ENEMY},
        {"tower", TokenType::TOWER},
        {"wave", TokenType::WAVE},
        {"spawn", TokenType::SPAWN},
        {"place", TokenType::PLACE},
        {"at", TokenType::AT},
        {"size", TokenType::SIZE},
        {"path", TokenType::PATH},
        {"count", TokenType::COUNT},
        {"start", TokenType::START},
        {"interval", TokenType::INTERVAL},
    };
}

char Lexer::peek() {
    return isAtEnd() ? '\0' : source[position];
}

char Lexer::advance() {
    return isAtEnd() ? '\0' : source[position++];
}

bool Lexer::isAtEnd() {
    return position >= source.size();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char currentChar = peek();
        if (currentChar == ' ' || currentChar == '\t' || currentChar == '\r') {
            advance();
        } else if (currentChar == '\n') {
            currentLine++;
            advance();
        } else {
            break;
        }
    }
}

void Lexer::skipComment() {
    // Single-line comments starting with //
    if (peek() == '/' && position + 1 < source.size() && source[position + 1] == '/') {
        while (!isAtEnd() && peek() != '\n')
            advance();
    }
}

Token Lexer::identifier() {
    size_t startPosition = position - 1;
    while (isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(startPosition, position - startPosition);
    if (keywords.count(text))
        return Token(keywords[text], text, currentLine);

    return Token(TokenType::IDENT, text, currentLine);
}

Token Lexer::number() {
    size_t startPosition = position - 1;
    bool isFloat = false;

    while (isdigit(peek())) advance();

    if (peek() == '.') {
        isFloat = true;
        advance();
        while (isdigit(peek())) advance();
    }

    std::string text = source.substr(startPosition, position - startPosition);
    return Token(isFloat ? TokenType::FLOAT : TokenType::INT, text, currentLine);
}

Token Lexer::getNextToken() {
    while (true) {
        skipWhitespace();
        skipComment();
        skipWhitespace();

        if (isAtEnd()) return Token(TokenType::END_OF_FILE, "", currentLine);

        char currentChar = advance();

        if (isdigit(currentChar))
            return number();
        if (isalpha(currentChar) || currentChar == '_')
            return identifier();

        switch (currentChar) {
            case '{': return Token(TokenType::LBRACE, "{", currentLine);
            case '}': return Token(TokenType::RBRACE, "}", currentLine);
            case '(': return Token(TokenType::LPAREN, "(", currentLine);
            case ')': return Token(TokenType::RPAREN, ")", currentLine);
            case '[': return Token(TokenType::LBRACKET, "[", currentLine);
            case ']': return Token(TokenType::RBRACKET, "]", currentLine);
            case ',': return Token(TokenType::COMMA, ",", currentLine);
            case ';': return Token(TokenType::SEMICOLON, ";", currentLine);
            case '=': return Token(TokenType::EQUAL, "=", currentLine);
        }

        return Token(TokenType::UNKNOWN, std::string(1, currentChar), currentLine);
    }
}

Token Lexer::peekToken() {
    size_t oldPosition = position;
    int oldLine = currentLine;
    Token token = getNextToken();
    position = oldPosition;
    currentLine = oldLine;
    return token;
}
