#ifndef IR_HPP
#define IR_HPP

#include "ast.hpp"
#include <vector>
#include <string>
#include <memory>
#include <variant>
#include <map>

// Intermediate Representation (IR) instruction opcodes
enum class IrOpcode {
    DEFINE_MAP,      // Define a new map
    DEFINE_ENEMY,    // Define a new enemy type
    DEFINE_TOWER,    // Define a new tower type
    DEFINE_WAVE,     // Define a new wave
    SPAWN_ENEMY,     // Spawn enemies in a wave
    PLACE_TOWER,     // Place a tower on the map
    SET_VALUE,       // Set a runtime value
    LOAD_CONST,      // Load a constant value
    NOP              // No operation (for optimization)
};

// Single IR instruction with opcode, operands, and metadata
struct IrInstruction {
    IrOpcode opcode;     // Operation to perform
    std::vector<std::string> operands;  // Instruction arguments
    std::map<std::string, std::variant<int, double, std::string>> metadata;  // Additional data

    IrInstruction() : opcode(IrOpcode::NOP) {}
    IrInstruction(IrOpcode op) : opcode(op) {}
};

// Generates IR from AST
class IrGenerator {
public:
    // Generate intermediate code from AST
    std::vector<IrInstruction> generate(std::shared_ptr<Program> program);

    // Convert IR instructions to human-readable format
    std::vector<std::string> toString(const std::vector<IrInstruction>& instructions);

private:
    std::vector<IrInstruction> code;  // Generated IR code

    // Add an instruction to the IR stream
    void emit(const IrInstruction& instruction) { code.push_back(instruction); }
};

#endif // IR_H
