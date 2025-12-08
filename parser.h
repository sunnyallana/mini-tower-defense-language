#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

// Syntax Analyzer - builds AST from token stream
class Parser {
public:
    Parser(Lexer& lexer);

    // Parse entire program
    std::shared_ptr<Program> parseProgram();

private:
    Lexer& lexer;           // Reference to lexer for token stream
    Token currentToken;     // Current token being processed

    // Helper methods
    void advance();                                  // Move to next token
    bool match(TokenType type);                      // Check and consume token if matches
    Token expect(TokenType type, const std::string& errorMessage);  // Require specific token

    // Parse individual declaration types
    std::shared_ptr<AstNode> parseDeclaration();
    std::shared_ptr<MapDecl> parseMapDecl();
    std::shared_ptr<EnemyDecl> parseEnemyDecl();
    std::shared_ptr<TowerDecl> parseTowerDecl();
    std::shared_ptr<WaveDecl> parseWaveDecl();
    std::shared_ptr<PlaceStmt> parsePlaceStmt();
};

#endif
