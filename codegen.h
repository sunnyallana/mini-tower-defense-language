#ifndef CODEGEN_H
#define CODEGEN_H

#include "ir.h"
#include <string>
#include <vector>

// Generates final output from optimized IR
class CodeGenerator {
public:
    // Generate JSON configuration from IR
    std::string generateJSON(const std::vector<IrInstruction>& instructions);

    // Generate human-readable text output from IR
    std::string generateReadable(const std::vector<IrInstruction>& instructions);

private:
    // JSON helper functions
    std::string escapeJSON(const std::string& str);
    std::string generateMapJSON(const IrInstruction& instruction);
    std::string generateEnemyJSON(const IrInstruction& instruction);
    std::string generateTowerJSON(const IrInstruction& instruction);
    std::string generateWaveJSON(const std::vector<IrInstruction>& instructions, size_t& index);
    std::string generatePlacementJSON(const IrInstruction& instruction);
};

#endif // CODEGEN_H
