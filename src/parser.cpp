#include "mtdl/parser.hpp"
#include <iostream>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    currentToken = lexer.getNextToken();
}

void Parser::advance() {
    currentToken = lexer.getNextToken();
}

bool Parser::match(TokenType type) {
    if (currentToken.type == type) {
        advance();
        return true;
    }
    return false;
}

Token Parser::expect(TokenType type, const std::string& errorMessage) {
    if (currentToken.type != type) {
        std::cerr << "Parser Error: expected " << errorMessage
                  << " at line " << currentToken.line << std::endl;
        exit(1);
    }
    Token token = currentToken;
    advance();
    return token;
}

std::shared_ptr<Program> Parser::parseProgram() {
    auto program = std::make_shared<Program>();
    while (currentToken.type != TokenType::END_OF_FILE) {
        program->declarations.push_back(parseDeclaration());
    }
    return program;
}

std::shared_ptr<AstNode> Parser::parseDeclaration() {
    if (match(TokenType::MAP)) return parseMapDecl();
    if (match(TokenType::ENEMY)) return parseEnemyDecl();
    if (match(TokenType::TOWER)) return parseTowerDecl();
    if (match(TokenType::WAVE)) return parseWaveDecl();
    if (match(TokenType::PLACE)) return parsePlaceStmt();

    std::cerr << "Unexpected declaration at line " << currentToken.line << "\n";
    exit(1);
}

std::shared_ptr<MapDecl> Parser::parseMapDecl() {
    auto node = std::make_shared<MapDecl>();
    Token nameToken = expect(TokenType::IDENT, "map name");
    node->name = nameToken.lexeme;

    expect(TokenType::LBRACE, "{");

    // Parse size: size = (width, height)
    expect(TokenType::SIZE, "size");
    expect(TokenType::EQUAL, "=");
    expect(TokenType::LPAREN, "(");
    Token widthToken = expect(TokenType::INT, "map width");
    node->width = std::stoi(widthToken.lexeme);
    expect(TokenType::COMMA, ",");
    Token heightToken = expect(TokenType::INT, "map height");
    node->height = std::stoi(heightToken.lexeme);
    expect(TokenType::RPAREN, ")");
    expect(TokenType::SEMICOLON, ";");

    // Parse path: path = [(x1,y1), (x2,y2), ...]
    expect(TokenType::PATH, "path");
    expect(TokenType::EQUAL, "=");
    expect(TokenType::LBRACKET, "[");
    while (!match(TokenType::RBRACKET)) {
        expect(TokenType::LPAREN, "(");
        Token xToken = expect(TokenType::INT, "x coordinate");
        int x = std::stoi(xToken.lexeme);
        expect(TokenType::COMMA, ",");
        Token yToken = expect(TokenType::INT, "y coordinate");
        int y = std::stoi(yToken.lexeme);
        expect(TokenType::RPAREN, ")");
        node->path.push_back({x, y});
        match(TokenType::COMMA);  // Optional comma between coordinates
    }
    expect(TokenType::SEMICOLON, ";");
    expect(TokenType::RBRACE, "}");
    return node;
}

std::shared_ptr<EnemyDecl> Parser::parseEnemyDecl() {
    auto node = std::make_shared<EnemyDecl>();
    Token nameToken = expect(TokenType::IDENT, "enemy name");
    node->name = nameToken.lexeme;

    expect(TokenType::LBRACE, "{");

    // Parse hp attribute
    Token hpToken = expect(TokenType::IDENT, "hp");
    expect(TokenType::EQUAL, "=");
    Token hpValueToken = expect(TokenType::INT, "hp value");
    node->hp = std::stoi(hpValueToken.lexeme);
    expect(TokenType::SEMICOLON, ";");

    // Parse speed attribute
    Token speedTokenName = expect(TokenType::IDENT, "speed");
    expect(TokenType::EQUAL, "=");
    Token speedValueToken = expect(TokenType::FLOAT, "speed value");
    node->speed = std::stod(speedValueToken.lexeme);
    expect(TokenType::SEMICOLON, ";");

    // Parse reward attribute
    Token rewardTokenName = expect(TokenType::IDENT, "reward");
    expect(TokenType::EQUAL, "=");
    Token rewardValueToken = expect(TokenType::INT, "reward value");
    node->reward = std::stoi(rewardValueToken.lexeme);
    expect(TokenType::SEMICOLON, ";");

    expect(TokenType::RBRACE, "}");
    return node;
}

std::shared_ptr<TowerDecl> Parser::parseTowerDecl() {
    auto node = std::make_shared<TowerDecl>();
    Token nameToken = expect(TokenType::IDENT, "tower name");
    node->name = nameToken.lexeme;

    expect(TokenType::LBRACE, "{");

    // Parse range attribute
    Token rangeTokenName = expect(TokenType::IDENT, "range");
    expect(TokenType::EQUAL, "=");
    Token rangeValueToken = expect(TokenType::INT, "range value");
    node->range = std::stoi(rangeValueToken.lexeme);
    expect(TokenType::SEMICOLON, ";");

    // Parse damage attribute
    Token damageTokenName = expect(TokenType::IDENT, "damage");
    expect(TokenType::EQUAL, "=");
    Token damageValueToken = expect(TokenType::INT, "damage value");
    node->damage = std::stoi(damageValueToken.lexeme);
    expect(TokenType::SEMICOLON, ";");

    // Parse fire rate attribute
    Token fireRateTokenName = expect(TokenType::IDENT, "fire_rate");
    expect(TokenType::EQUAL, "=");
    Token fireRateValueToken = expect(TokenType::FLOAT, "fire_rate value");
    node->fireRate = std::stod(fireRateValueToken.lexeme);
    expect(TokenType::SEMICOLON, ";");

    // Parse cost attribute
    Token costTokenName = expect(TokenType::IDENT, "cost");
    expect(TokenType::EQUAL, "=");
    Token costValueToken = expect(TokenType::INT, "cost value");
    node->cost = std::stoi(costValueToken.lexeme);
    expect(TokenType::SEMICOLON, ";");

    expect(TokenType::RBRACE, "}");
    return node;
}

std::shared_ptr<WaveDecl> Parser::parseWaveDecl() {
    auto node = std::make_shared<WaveDecl>();
    Token nameToken = expect(TokenType::IDENT, "wave name");
    node->name = nameToken.lexeme;

    expect(TokenType::LBRACE, "{");

    // Parse multiple spawn statements
    while (match(TokenType::SPAWN)) {
        SpawnStmt spawn;
        expect(TokenType::LPAREN, "(");

        Token enemyToken = expect(TokenType::IDENT, "enemy type");
        spawn.enemyType = enemyToken.lexeme;

        expect(TokenType::COMMA, ",");
        expect(TokenType::COUNT, "count");
        expect(TokenType::EQUAL, "=");
        Token countToken = expect(TokenType::INT, "count");
        spawn.count = std::stoi(countToken.lexeme);

        expect(TokenType::COMMA, ",");
        expect(TokenType::START, "start");
        expect(TokenType::EQUAL, "=");
        Token startToken = expect(TokenType::INT, "start");
        spawn.start = std::stoi(startToken.lexeme);

        expect(TokenType::COMMA, ",");
        expect(TokenType::INTERVAL, "interval");
        expect(TokenType::EQUAL, "=");
        Token intervalToken = expect(TokenType::INT, "interval");
        spawn.interval = std::stoi(intervalToken.lexeme);

        expect(TokenType::RPAREN, ")");
        expect(TokenType::SEMICOLON, ";");

        node->spawns.push_back(spawn);
    }

    expect(TokenType::RBRACE, "}");
    return node;
}

std::shared_ptr<PlaceStmt> Parser::parsePlaceStmt() {
    auto node = std::make_shared<PlaceStmt>();
    Token towerToken = expect(TokenType::IDENT, "tower type");
    node->towerType = towerToken.lexeme;

    expect(TokenType::AT, "at");
    expect(TokenType::LPAREN, "(");

    Token xToken = expect(TokenType::INT, "x coordinate");
    node->x = std::stoi(xToken.lexeme);

    expect(TokenType::COMMA, ",");

    Token yToken = expect(TokenType::INT, "y coordinate");
    node->y = std::stoi(yToken.lexeme);

    expect(TokenType::RPAREN, ")");
    expect(TokenType::SEMICOLON, ";");

    return node;
}
