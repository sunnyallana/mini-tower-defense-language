#include "optimizer.h"
#include <iostream>
#include <algorithm>

std::vector<IrInstruction> Optimizer::optimize(const std::vector<IrInstruction>& instructions) {
    auto result = instructions;

    // Apply optimization passes in sequence
    std::cout << "Running optimization passes...\n";

    // Pass 1: Remove duplicate definitions (keep first occurrence)
    result = duplicateDefinitionRemoval(result);

    // Pass 2: Merge redundant spawns in same wave
    result = redundantSpawnMerging(result);

    // Pass 3: Constant folding (for any computed values)
    result = constantFolding(result);

    // Pass 4: Dead code elimination
    result = deadCodeElimination(result);

    std::cout << "Optimization complete.\n";
    return result;
}

std::vector<IrInstruction> Optimizer::constantFolding(const std::vector<IrInstruction>& instructions) {
    std::vector<IrInstruction> optimized;

    for (const auto& instruction : instructions) {
        IrInstruction newInstruction = instruction;

        // Optimize constant expressions in metadata
        // For tower defense game, we can pre-calculate DPS, total wave spawn times, etc.
        if (instruction.opcode == IrOpcode::DEFINE_TOWER) {
            // Calculate and store DPS (Damage Per Second)
            if (instruction.metadata.count("damage") && instruction.metadata.count("fire_rate")) {
                int damage = std::get<int>(instruction.metadata.at("damage"));
                double fireRate = std::get<double>(instruction.metadata.at("fire_rate"));
                double dps = damage * fireRate;
                newInstruction.metadata["dps"] = dps;
            }
        }

        if (instruction.opcode == IrOpcode::SPAWN_ENEMY) {
            // Calculate total spawn duration
            if (instruction.metadata.count("count") && instruction.metadata.count("interval")) {
                int count = std::get<int>(instruction.metadata.at("count"));
                int interval = std::get<int>(instruction.metadata.at("interval"));
                int totalDuration = count * interval;
                newInstruction.metadata["total_duration"] = totalDuration;
            }
        }

        optimized.push_back(newInstruction);
    }

    return optimized;
}

std::vector<IrInstruction> Optimizer::deadCodeElimination(const std::vector<IrInstruction>& instructions) {
    std::vector<IrInstruction> optimized;
    std::set<std::string> referencedEnemies;
    std::set<std::string> referencedTowers;
    std::set<std::string> definedEnemies;
    std::set<std::string> definedTowers;

    // First pass: collect all references and definitions
    for (const auto& instruction : instructions) {
        if (instruction.opcode == IrOpcode::DEFINE_ENEMY && !instruction.operands.empty()) {
            definedEnemies.insert(instruction.operands[0]);
        }
        if (instruction.opcode == IrOpcode::DEFINE_TOWER && !instruction.operands.empty()) {
            definedTowers.insert(instruction.operands[0]);
        }
        if (instruction.opcode == IrOpcode::SPAWN_ENEMY && instruction.operands.size() > 1) {
            referencedEnemies.insert(instruction.operands[1]);
        }
        if (instruction.opcode == IrOpcode::PLACE_TOWER && !instruction.operands.empty()) {
            referencedTowers.insert(instruction.operands[0]);
        }
    }

    // Second pass: keep only referenced definitions
    for (const auto& instruction : instructions) {
        bool keep = true;

        // Remove unreferenced enemy definitions
        if (instruction.opcode == IrOpcode::DEFINE_ENEMY && !instruction.operands.empty()) {
            if (referencedEnemies.find(instruction.operands[0]) == referencedEnemies.end()) {
                std::cout << "  DCE: Removing unreferenced enemy: " << instruction.operands[0] << "\n";
                keep = false;
            }
        }

        // Remove unreferenced tower definitions
        if (instruction.opcode == IrOpcode::DEFINE_TOWER && !instruction.operands.empty()) {
            if (referencedTowers.find(instruction.operands[0]) == referencedTowers.end()) {
                std::cout << "  DCE: Removing unreferenced tower: " << instruction.operands[0] << "\n";
                keep = false;
            }
        }

        // Skip NOP instructions
        if (instruction.opcode == IrOpcode::NOP) {
            keep = false;
        }

        if (keep) {
            optimized.push_back(instruction);
        }
    }

    return optimized;
}

std::vector<IrInstruction> Optimizer::duplicateDefinitionRemoval(const std::vector<IrInstruction>& instructions) {
    std::vector<IrInstruction> optimized;
    std::set<std::string> seenDefinitions;

    for (const auto& instruction : instructions) {
        bool keep = true;

        if (isDefinitionInstruction(instruction.opcode) && !instruction.operands.empty()) {
            std::string key = getDefinitionKey(instruction);

            if (seenDefinitions.find(key) != seenDefinitions.end()) {
                std::cout << "  Optimization: Removing duplicate definition: " << key << "\n";
                keep = false;
            } else {
                seenDefinitions.insert(key);
            }
        }

        if (keep) {
            optimized.push_back(instruction);
        }
    }

    return optimized;
}

std::vector<IrInstruction> Optimizer::redundantSpawnMerging(const std::vector<IrInstruction>& instructions) {
    std::vector<IrInstruction> optimized;
    std::map<std::string, size_t> spawnGroupIndex; // Key -> index in optimized

    for (const auto& instruction : instructions) {
        if (instruction.opcode == IrOpcode::SPAWN_ENEMY && instruction.operands.size() >= 2) {
            std::string wave = instruction.operands[0];
            std::string enemy = instruction.operands[1];
            int start = std::get<int>(instruction.metadata.at("start"));
            int interval = std::get<int>(instruction.metadata.at("interval"));

            // Create unique key for spawn grouping
            std::string key = wave + "_" + enemy + "_" + std::to_string(start) + "_" + std::to_string(interval);

            if (spawnGroupIndex.find(key) != spawnGroupIndex.end()) {
                // Merge: update existing spawn in-place
                size_t index = spawnGroupIndex[key];
                int existingCount = std::get<int>(optimized[index].metadata.at("count"));
                int newCount = std::get<int>(instruction.metadata.at("count"));
                optimized[index].metadata["count"] = existingCount + newCount;
                std::cout << "  Optimization: Merged redundant spawn in wave " << wave << "\n";
            } else {
                spawnGroupIndex[key] = optimized.size();
                optimized.push_back(instruction);
            }
        } else {
            optimized.push_back(instruction);
        }
    }

    return optimized;
}

bool Optimizer::isDefinitionInstruction(IrOpcode opcode) {
    return opcode == IrOpcode::DEFINE_MAP ||
           opcode == IrOpcode::DEFINE_ENEMY ||
           opcode == IrOpcode::DEFINE_TOWER ||
           opcode == IrOpcode::DEFINE_WAVE;
}

std::string Optimizer::getDefinitionKey(const IrInstruction& instruction) {
    std::string prefix;
    switch (instruction.opcode) {
        case IrOpcode::DEFINE_MAP: prefix = "MAP:"; break;
        case IrOpcode::DEFINE_ENEMY: prefix = "ENEMY:"; break;
        case IrOpcode::DEFINE_TOWER: prefix = "TOWER:"; break;
        case IrOpcode::DEFINE_WAVE: prefix = "WAVE:"; break;
        default: prefix = "UNKNOWN:"; break;
    }
    return prefix + (instruction.operands.empty() ? "" : instruction.operands[0]);
}
