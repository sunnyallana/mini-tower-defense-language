#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include <unordered_map>
#include <string>

// Semantic Analyzer - validates program meaning and consistency
class SemanticAnalyzer {
public:
    void analyze(std::shared_ptr<Program> program);

private:
    // Symbol tables for each declaration type
    std::unordered_map<std::string, MapDecl*> mapDeclarations;
    std::unordered_map<std::string, EnemyDecl*> enemyDeclarations;
    std::unordered_map<std::string, TowerDecl*> towerDeclarations;
    std::unordered_map<std::string, WaveDecl*> waveDeclarations;

    MapDecl* currentMap = nullptr;  // Track current map for placement validation

    // Validation methods for each AST node type
    void checkMap(MapDecl* map);
    void checkEnemy(EnemyDecl* enemy);
    void checkTower(TowerDecl* tower);
    void checkWave(WaveDecl* wave);
    void checkPlacement(PlaceStmt* placement);
};

#endif
