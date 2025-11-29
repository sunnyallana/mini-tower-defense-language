
# 1. Introduction

Modern video games frequently employ scripting languages to define critical elements like level behavior, enemy wave configurations, and core game rules. This project proposes TDLang (Mini Tower Defense Language, or MTDL), a small domain-specific language (DSL) specifically designed for scripting levels in a Tower Defense game.

TDLang will allow designers to clearly define:
- Maps and tower/enemy attributes.
- Enemy waves and their spawn logic.
- Tower and enemy placements.
- Simple conditional behaviors for game events.

This endeavor integrates the theoretical concepts of compiler construction with a practical application in game design.

# 2. Objectives and Scope

The primary objectives for this project are focused on developing a fully functional compiler for MTDL:

- **Custom Language Definition**: To define a mini-language syntax and semantics for describing towers, enemies, waves, and in-game actions.

- **Full Compiler Implementation**: To implement all six standard phases of a compiler:
   - **Lexical Analysis**: Tokenizing the input script.
   - **Syntax Analysis**: Parsing the tokens to ensure grammatical correctness.
   - **Semantic Analysis**: Checking for meaning and type consistency.
   - **Intermediate Code Generation**: Creating an abstract representation of the script.
   - **Optimization**: Applying techniques to improve the generated code.
   - **Final Code Generation**: Producing the interpretable output.

- **Script Interpretation**: To enable users to write MTDL scripts that define tower attributes, enemy spawn patterns, and basic game rules. The compiler must parse these scripts and generate interpretable pseudo-code capable of simulating the core tower defense behavior.

- **Robust Error Reporting**: To provide clear and informative error messages for lexical, syntax, and semantic issues, thereby assisting users in correcting their script mistakes.

# 3. Tools and Technologies

The MTDL compiler will be primarily implemented using the C++ programming language.

The potential tools and components include:
- A handwritten lexical analyzer (utilizing a Flex-style approach).
- A handwritten recursive-descent parser for syntax analysis.
- A Symbol Table and an Intermediate Representation mechanism for code management.

The final compiled program will be compatible with any standard x86 system.

# 4. Core Features and Capabilities

The key features of the Mini Tower Defense Language (MTDL) compiler will include:

- **Tower Definition**: The ability to define essential tower attributes such as range, damage, and cost.

- **Enemy Wave Definition**: The capability to define enemy parameters including speed, health, and spawn rate for different waves.

- **Custom Event Handling**: Support for defining logic tied to common in-game events, such as `onHit`, `onDeath`, or `onWaveStart`.

- **Simulation Code Output**: Generation of intermediate code necessary for driving the tower defense simulation.

- **Code Optimization**: Implementation of fundamental optimizations, such as constant folding and dead rule elimination.

- **Comprehensive Error Reporting**: Detailed reporting for all three major classes of errors: lexical, syntax, and semantic.
```
