#include "semantic.h"
#include <iostream>
#include <set>

void SemanticAnalyzer::analyze(std::shared_ptr<Program> program) {
    for (auto declaration : program->declarations) {
        if (auto mapDecl = dynamic_cast<MapDecl*>(declaration.get())) {
            checkMap(mapDecl);
        }
        else if (auto enemyDecl = dynamic_cast<EnemyDecl*>(declaration.get())) {
            checkEnemy(enemyDecl);
        }
        else if (auto towerDecl = dynamic_cast<TowerDecl*>(declaration.get())) {
            checkTower(towerDecl);
        }
        else if (auto waveDecl = dynamic_cast<WaveDecl*>(declaration.get())) {
            checkWave(waveDecl);
        }
        else if (auto placeStmt = dynamic_cast<PlaceStmt*>(declaration.get())) {
            checkPlacement(placeStmt);
        }
    }
}

void SemanticAnalyzer::checkMap(MapDecl* map) {
    // Check for duplicate map names
    if (mapDeclarations.count(map->name)) {
        std::cerr << "Semantic Error: Duplicate map name " << map->name << "\n";
        exit(1);
    }
    mapDeclarations[map->name] = map;
    currentMap = map;

    // Validate map dimensions
    if (map->width <= 0 || map->height <= 0) {
        std::cerr << "Invalid map size.\n";
        exit(1);
    }

    // Validate all path coordinates are within map bounds
    for (auto& point : map->path) {
        if (point.first < 0 || point.first >= map->width ||
            point.second < 0 || point.second >= map->height) {
            std::cerr << "Path coordinate out of map bounds.\n";
            exit(1);
        }
    }
}

void SemanticAnalyzer::checkEnemy(EnemyDecl* enemy) {
    if (enemyDeclarations.count(enemy->name)) {
        std::cerr << "Duplicate enemy: " << enemy->name << "\n";
        exit(1);
    }
    enemyDeclarations[enemy->name] = enemy;

    // Validate enemy attributes
    if (enemy->hp <= 0) {
        std::cerr << "Enemy HP must be positive.\n";
        exit(1);
    }
    if (enemy->speed <= 0) {
        std::cerr << "Enemy speed must be positive.\n";
        exit(1);
    }
    if (enemy->reward < 0) {
        std::cerr << "Enemy reward cannot be negative.\n";
        exit(1);
    }
}

void SemanticAnalyzer::checkTower(TowerDecl* tower) {
    if (towerDeclarations.count(tower->name)) {
        std::cerr << "Duplicate tower: " << tower->name << "\n";
        exit(1);
    }
    towerDeclarations[tower->name] = tower;

    // Validate tower attributes
    if (tower->range <= 0 || tower->damage <= 0 || tower->cost < 0) {
        std::cerr << "Invalid tower stats.\n";
        exit(1);
    }
    if (tower->fireRate <= 0) {
        std::cerr << "Tower fire rate must be positive.\n";
        exit(1);
    }
}

void SemanticAnalyzer::checkWave(WaveDecl* wave) {
    if (waveDeclarations.count(wave->name)) {
        std::cerr << "Duplicate wave: " << wave->name << "\n";
        exit(1);
    }
    waveDeclarations[wave->name] = wave;

    // Validate each spawn in the wave
    for (auto& spawn : wave->spawns) {
        if (!enemyDeclarations.count(spawn.enemyType)) {
            std::cerr << "Wave uses undefined enemy: " << spawn.enemyType << "\n";
            exit(1);
        }
        if (spawn.count <= 0 || spawn.start < 0 || spawn.interval <= 0) {
            std::cerr << "Invalid spawn parameters.\n";
            exit(1);
        }
    }
}

void SemanticAnalyzer::checkPlacement(PlaceStmt* placement) {
    if (!towerDeclarations.count(placement->towerType)) {
        std::cerr << "Placing undefined tower type: " << placement->towerType << "\n";
        exit(1);
    }

    // Ensure a map has been defined before placement statements
    if (!currentMap) {
        std::cerr << "Place statement appears before map definition.\n";
        exit(1);
    }

    // Validate placement coordinates are within map bounds
    if (placement->x < 0 || placement->x >= currentMap->width ||
        placement->y < 0 || placement->y >= currentMap->height) {
        std::cerr << "Tower placement out of map bounds.\n";
        exit(1);
    }
}
