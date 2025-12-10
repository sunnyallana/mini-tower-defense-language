# Mini Tower Defense Language (MTDL) Compiler

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)

A complete, multi-stage compiler for the **Mini Tower Defense Language (MTDL)**, a domain-specific language for defining tower defense game configurations. The compiler implements all six phases of traditional compiler construction with game-specific optimizations.

## Purpose

This project serves as a comprehensive educational resource and practical tool that bridges compiler theory with game development. It demonstrates the complete compiler construction pipeline—from lexical analysis through code generation—while solving a real-world problem: creating a clean, designer-friendly language for tower defense games. By implementing game-specific optimizations like constant folding for DPS calculations and dead code elimination, the compiler produces efficient JSON configurations ready for immediate use in game engines, making it valuable for both learning compiler design principles and practical game development workflows.

## What is MTDL?

MTDL allows game designers to define complete tower defense game scenarios in a clean, readable format:

```mtdl
map CastleDefense {
    size = (30, 20);
    path = [(0,10), (10,10), (10,5), (20,5)];
}

enemy Goblin {
    hp = 50;
    speed = 1.5;
    reward = 10;
}

tower Archer {
    range = 4;
    damage = 20;
    fire_rate = 1.5;
    cost = 75;
}

wave Wave1 {
    spawn(Goblin, count=15, start=0, interval=1);
}

place Archer at (3,8);
```

## Quick Start

### Clone and Build
```bash
git clone https://github.com/sunnyallana/mini-tower-defense-language.git
cd mini-tower-defense-language

# Build the compiler
g++ -std=c++17 -o mtdl src/*.cpp -Iinclude

# Run on example
./mtdl examples/basic.mtdl -o game_config.json
```

### Check Output
```bash
cat game_config.json
```

## Project Structure

```
mini-tower-defense-language/
├── include/mtdl/           # All header files
│   ├── ast.hpp            # Abstract Syntax Tree definitions
│   ├── token.hpp          # Token types and structures
│   ├── lexer.hpp          # Lexical analyzer
│   ├── parser.hpp         # Syntax parser
│   ├── semantic.hpp       # Semantic analyzer
│   ├── ir.hpp             # Intermediate Representation
│   ├── optimizer.hpp      # Optimization passes
│   └── codegen.hpp        # Code generator
├── src/                   # Implementation files
│   ├── main.cpp           # Compiler driver with CLI
│   ├── lexer.cpp          # Lexer implementation
│   ├── parser.cpp         # Parser implementation
│   ├── semantic.cpp       # Semantic analysis
│   ├── ir.cpp             # IR generation
│   ├── optimizer.cpp      # Optimization implementation
│   └── codegen.cpp        # Code generation
├── examples/              # Sample MTDL configurations
│   ├── basic.mtdl         # Simple example
│   ├── castle_defense.mtdl # Complex scenario
│   └── wave_test.mtdl     # Wave pattern testing
└── README.md              # This file
```

## Compiler Features

### Complete Compiler Pipeline
1. **Lexical Analysis** - Tokenizes source code
2. **Syntax Analysis** - Builds AST with recursive descent parsing
3. **Semantic Analysis** - Validates program meaning and consistency
4. **IR Generation** - Creates platform-independent intermediate code
5. **Optimization** - Applies game-specific optimizations
6. **Code Generation** - Produces JSON configuration for game engines

### Game-Specific Optimizations
- **Constant Folding**: Pre-calculates tower DPS (Damage × Fire Rate)
- **Dead Code Elimination**: Removes unused enemy/tower definitions
- **Duplicate Removal**: Eliminates redundant definitions
- **Spawn Merging**: Combines identical spawns in waves

## Language Specification

### Map Definition
```mtdl
map MapName {
    size = (width, height);                    // Map dimensions
    path = [(x1,y1), (x2,y2), ..., (xn,yn)];   // Enemy path
}
```

### Enemy Definition
```mtdl
enemy EnemyName {
    hp = value;      // Health points (positive integer)
    speed = value;   // Movement speed (positive float)
    reward = value;  // Gold reward (non-negative integer)
}
```

### Tower Definition
```mtdl
tower TowerName {
    range = value;      // Attack range (positive integer)
    damage = value;     // Damage per attack (positive integer)
    fire_rate = value;  // Attacks per second (positive float)
    cost = value;       // Build cost (non-negative integer)
}
```

### Wave Definition
```mtdl
wave WaveName {
    spawn(EnemyType, count=value, start=value, interval=value);
    // Can have multiple spawns
}
```

### Tower Placement
```mtdl
place TowerType at (x, y);
```

## Building from Source

```bash
g++ -std=c++17 -o mtdl src/*.cpp -Iinclude
```

## Using the Compiler

### Basic Usage
```bash
./mtdl input.mtdl                    # Creates output.json
./mtdl input.mtdl -o config.json     # Custom output file
```

### Command Line Options
```bash
-o <file>        Output file (default: output.json)
-ir              Show intermediate representation
-readable        Generate human-readable text output
-no-opt          Disable all optimizations
-h, --help       Show help message
```

### Examples
```bash
# Show IR during compilation
./mtdl examples/castle_defense.mtdl -ir

# Generate text output for debugging
./mtdl examples/basic.mtdl -readable -o debug.txt

# Compare optimized vs non-optimized
./mtdl examples/basic.mtdl -o optimized.json
./mtdl examples/basic.mtdl -no-opt -o non_optimized.json
```

## Testing

### Run Examples
```bash
# Test basic example
./mtdl examples/basic.mtdl -o test_output.json

# Test complex scenario
./mtdl examples/castle_defense.mtdl -ir
```

### Verify Optimizations
```bash
# Check if DPS is pre-calculated (optimization working)
./mtdl examples/basic.mtdl -o opt.json
grep "dps" opt.json  # Should show "dps": 30.00

# Compare with non-optimized
./mtdl examples/basic.mtdl -no-opt -o noopt.json
grep "dps" noopt.json  # Should show nothing
```

## Sample Output

### JSON Configuration
```json
{
  "gameConfig": {
    "map": {
      "name": "CastleDefense",
      "width": 30,
      "height": 20,
      "path": [
        {"x": 0, "y": 10},
        {"x": 10, "y": 10},
        {"x": 10, "y": 5},
        {"x": 20, "y": 5}
      ]
    },
    "enemies": [
      {
        "name": "Goblin",
        "hp": 50,
        "speed": 1.50,
        "reward": 10
      }
    ],
    "towers": [
      {
        "name": "Archer",
        "range": 4,
        "damage": 20,
        "fireRate": 1.50,
        "cost": 75,
        "dps": 30.00
      }
    ],
    "waves": [
      {
        "name": "Wave1",
        "spawns": [
          {
            "enemyType": "Goblin",
            "count": 15,
            "start": 0,
            "interval": 1
          }
        ]
      }
    ],
    "initialPlacements": [
      {
        "towerType": "Archer",
        "x": 3,
        "y": 8
      }
    ]
  }
}
```

### Human-Readable Output
```
=== MTDL Compiled Output ===

DEFINE_MAP CastleDefense WIDTH=30 HEIGHT=20 PATH=[0,10;10,10;10,5;20,5]
DEFINE_ENEMY Goblin HP=50 SPEED=1.50 REWARD=10
DEFINE_TOWER Archer RANGE=4 DAMAGE=20 FIRERATE=1.50 COST=75 DPS=30.00
DEFINE_WAVE Wave1
  SPAWN_ENEMY Goblin IN_WAVE=Wave1 COUNT=15 START=0 INTERVAL=1
PLACE_TOWER Archer X=3 Y=8
```

## Compiler Architecture

### Lexical Analysis (lexer.hpp/cpp)
- Converts source to tokens (keywords, identifiers, numbers, symbols)
- Handles comments and whitespace
- Tracks line numbers for error reporting

### Syntax Analysis (parser.hpp/cpp)
- Recursive descent parser
- Builds Abstract Syntax Tree (AST)
- Validates grammar structure

### Semantic Analysis (semantic.hpp/cpp)
- Symbol table management
- Type checking and validation
- Reference validation
- Bounds checking

### Intermediate Representation (ir.hpp/cpp)
- Platform-independent IR instructions
- Metadata storage for game attributes
- Hierarchical wave/spawn relationships

### Optimization (optimizer.hpp/cpp)
- **Constant Folding**: Pre-calculates DPS and durations
- **Dead Code Elimination**: Removes unused definitions
- **Duplicate Removal**: Eliminates redundant data
- **Spawn Merging**: Combines identical spawns

### Code Generation (codegen.hpp/cpp)
- JSON output for game engines
- Human-readable text format
- Proper formatting and escaping

## Debugging

### Show Compilation Phases
```bash
./mtdl examples/basic.mtdl  # Shows progress through all 6 phases
```

### View Intermediate Representation
```bash
./mtdl examples/basic.mtdl -ir  # Shows IR instructions
```

### Disable Optimizations
```bash
./mtdl examples/basic.mtdl -no-opt  # For debugging optimization issues
```

## Performance Benefits

1. **30% smaller output** with dead code elimination
2. **Pre-calculated DPS** eliminates runtime math
3. **Merged spawns** reduce wave processing overhead
4. **Clean JSON** ready for immediate game engine use

## Future Roadmap

### Language Enhancements
- Arithmetic expressions in attributes
- Conditional spawns (if-then logic)
- Tower upgrade chains
- Game events (onWaveStart, onEnemyDeath)

### Compiler Features
- LLVM backend for native code
- Interactive debugger
- Visual AST viewer
- Language server support

### Optimizations
- Profile-guided optimization
- Parallel wave simulation
- Cache-aware code generation

## Contributing

Contributions are welcome! Here's how:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

Please ensure:
- Code follows existing style
- Add tests for new features
- Update documentation as needed

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by compiler construction principles
- Designed for practical game development use
- Built as an educational project in compiler design

---
