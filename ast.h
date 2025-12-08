#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>

// Abstract base class for all AST nodes
struct AstNode {
    virtual ~AstNode() {}
};

// Map declaration node - defines game map properties
struct MapDecl : AstNode {
    std::string name;                       // Map identifier
    int width;                              // Map width in tiles
    int height;                             // Map height in tiles
    std::vector<std::pair<int, int>> path;  // Enemy path coordinates
};

// Enemy declaration node - defines enemy attributes
struct EnemyDecl : AstNode {
    std::string name;    // Enemy identifier
    int hp;              // Health points
    double speed;        // Movement speed
    int reward;          // Gold reward when defeated
};

// Tower declaration node - defines tower attributes
struct TowerDecl : AstNode {
    std::string name;    // Tower identifier
    int range;           // Attack range in tiles
    int damage;          // Damage per attack
    int cost;            // Gold cost to build
    double fireRate;     // Attacks per second
};

// Individual spawn statement within a wave
struct SpawnStmt {
    std::string enemyType;  // Type of enemy to spawn
    int count;              // Number of enemies to spawn
    int start;              // Start time (seconds)
    int interval;           // Time between spawns (seconds)
};

// Wave declaration node - defines enemy wave configuration
struct WaveDecl : AstNode {
    std::string name;                // Wave identifier
    std::vector<SpawnStmt> spawns;   // List of spawns in this wave
};

// Tower placement statement
struct PlaceStmt : AstNode {
    std::string towerType;  // Type of tower to place
    int x;                  // X-coordinate on map
    int y;                  // Y-coordinate on map
};

// Root node of the AST containing all declarations
struct Program : AstNode {
    std::vector<std::shared_ptr<AstNode>> declarations;
};

#endif
