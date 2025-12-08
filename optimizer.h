#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "ir.h"
#include <vector>
#include <set>
#include <map>

// Performs optimization passes on IR code
class Optimizer {
public:
    // Main optimization entry point
    std::vector<IrInstruction> optimize(const std::vector<IrInstruction>& instructions);

private:
    // Individual optimization passes
    std::vector<IrInstruction> constantFolding(const std::vector<IrInstruction>& instructions);
    std::vector<IrInstruction> deadCodeElimination(const std::vector<IrInstruction>& instructions);
    std::vector<IrInstruction> duplicateDefinitionRemoval(const std::vector<IrInstruction>& instructions);
    std::vector<IrInstruction> redundantSpawnMerging(const std::vector<IrInstruction>& instructions);

    // Helper functions
    bool isDefinitionInstruction(IrOpcode opcode);
    std::string getDefinitionKey(const IrInstruction& instruction);
};

#endif // OPTIMIZER_H
