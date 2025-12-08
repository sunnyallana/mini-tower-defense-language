#include "mtdl/codegen.hpp"
#include <sstream>
#include <iomanip>

std::string CodeGenerator::escapeJSON(const std::string& str) {
    std::ostringstream escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped << "\\\""; break;
            case '\\': escaped << "\\\\"; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;
            default: escaped << c; break;
        }
    }
    return escaped.str();
}

std::string CodeGenerator::generateMapJSON(const IrInstruction& instruction) {
    std::ostringstream json;
    json << "    \"map\": {\n";
    json << "      \"name\": \"" << escapeJSON(instruction.operands[0]) << "\",\n";

    if (instruction.metadata.count("width")) {
        json << "      \"width\": " << std::get<int>(instruction.metadata.at("width")) << ",\n";
    }
    if (instruction.metadata.count("height")) {
        json << "      \"height\": " << std::get<int>(instruction.metadata.at("height")) << ",\n";
    }

    if (instruction.metadata.count("path")) {
        std::string pathStr = std::get<std::string>(instruction.metadata.at("path"));
        json << "      \"path\": [\n";

        std::istringstream pathStream(pathStr);
        std::string coord;
        bool first = true;

        while (std::getline(pathStream, coord, ';')) {
            if (!first) json << ",\n";
            first = false;

            size_t commaPos = coord.find(',');
            if (commaPos != std::string::npos) {
                int x = std::stoi(coord.substr(0, commaPos));
                int y = std::stoi(coord.substr(commaPos + 1));
                json << "        {\"x\": " << x << ", \"y\": " << y << "}";
            }
        }

        json << "\n      ]\n";
    }

    json << "    }";
    return json.str();
}

std::string CodeGenerator::generateEnemyJSON(const IrInstruction& instruction) {
    std::ostringstream json;
    json << "      {\n";
    json << "        \"name\": \"" << escapeJSON(instruction.operands[0]) << "\",\n";

    if (instruction.metadata.count("hp")) {
        json << "        \"hp\": " << std::get<int>(instruction.metadata.at("hp")) << ",\n";
    }
    if (instruction.metadata.count("speed")) {
        json << "        \"speed\": " << std::fixed << std::setprecision(2)
             << std::get<double>(instruction.metadata.at("speed")) << ",\n";
    }
    if (instruction.metadata.count("reward")) {
        json << "        \"reward\": " << std::get<int>(instruction.metadata.at("reward")) << "\n";
    }

    json << "      }";
    return json.str();
}

std::string CodeGenerator::generateTowerJSON(const IrInstruction& instruction) {
    std::ostringstream json;
    json << "      {\n";
    json << "        \"name\": \"" << escapeJSON(instruction.operands[0]) << "\",\n";

    if (instruction.metadata.count("range")) {
        json << "        \"range\": " << std::get<int>(instruction.metadata.at("range")) << ",\n";
    }
    if (instruction.metadata.count("damage")) {
        json << "        \"damage\": " << std::get<int>(instruction.metadata.at("damage")) << ",\n";
    }
    if (instruction.metadata.count("fire_rate")) {
        json << "        \"fireRate\": " << std::fixed << std::setprecision(2)
             << std::get<double>(instruction.metadata.at("fire_rate")) << ",\n";
    }
    if (instruction.metadata.count("cost")) {
        json << "        \"cost\": " << std::get<int>(instruction.metadata.at("cost"));
    }

    // Include optimized DPS if available
    if (instruction.metadata.count("dps")) {
        json << ",\n        \"dps\": " << std::fixed << std::setprecision(2)
             << std::get<double>(instruction.metadata.at("dps"));
    }

    json << "\n      }";
    return json.str();
}

std::string CodeGenerator::generateWaveJSON(const std::vector<IrInstruction>& instructions, size_t& index) {
    std::ostringstream json;

    const IrInstruction& waveInstruction = instructions[index];
    json << "      {\n";
    json << "        \"name\": \"" << escapeJSON(waveInstruction.operands[0]) << "\",\n";
    json << "        \"spawns\": [\n";

    bool firstSpawn = true;
    size_t i = index + 1;

    // Collect all SPAWN_ENEMY instructions for this wave
    while (i < instructions.size() &&
            instructions[i].opcode == IrOpcode::SPAWN_ENEMY &&
            instructions[i].operands[0] == waveInstruction.operands[0]
        ) {
        const IrInstruction& spawnInstruction = instructions[i];

        if (!firstSpawn) json << ",\n";
        firstSpawn = false;

        json << "          {\n";
        json << "            \"enemyType\": \"" << escapeJSON(spawnInstruction.operands[1]) << "\",\n";

        if (spawnInstruction.metadata.count("count"))
            json << "            \"count\": " << std::get<int>(spawnInstruction.metadata.at("count")) << ",\n";
        if (spawnInstruction.metadata.count("start"))
            json << "            \"start\": " << std::get<int>(spawnInstruction.metadata.at("start")) << ",\n";
        if (spawnInstruction.metadata.count("interval"))
            json << "            \"interval\": " << std::get<int>(spawnInstruction.metadata.at("interval")) << "\n";

        json << "          }";
        i++;
    }

    json << "\n        ]\n";
    json << "      }";

    // Update index to the last spawn processed
    index = i - 1;
    return json.str();
}

std::string CodeGenerator::generatePlacementJSON(const IrInstruction& instruction) {
    std::ostringstream json;
    json << "      {\n";
    json << "        \"towerType\": \"" << escapeJSON(instruction.operands[0]) << "\",\n";

    if (instruction.metadata.count("x")) {
        json << "        \"x\": " << std::get<int>(instruction.metadata.at("x")) << ",\n";
    }
    if (instruction.metadata.count("y")) {
        json << "        \"y\": " << std::get<int>(instruction.metadata.at("y")) << "\n";
    }

    json << "      }";
    return json.str();
}

std::string CodeGenerator::generateJSON(const std::vector<IrInstruction>& instructions) {
    std::ostringstream json;

    json << "{\n";
    json << "  \"gameConfig\": {\n";

    bool hasMap = false;
    std::vector<size_t> enemyIndices;
    std::vector<size_t> towerIndices;
    std::vector<size_t> waveIndices;
    std::vector<size_t> placementIndices;

    // Categorize instructions by type
    for (size_t i = 0; i < instructions.size(); i++) {
        switch (instructions[i].opcode) {
            case IrOpcode::DEFINE_MAP:
                if (!hasMap) {
                    json << generateMapJSON(instructions[i]);
                    hasMap = true;
                }
                break;
            case IrOpcode::DEFINE_ENEMY:
                enemyIndices.push_back(i);
                break;
            case IrOpcode::DEFINE_TOWER:
                towerIndices.push_back(i);
                break;
            case IrOpcode::DEFINE_WAVE:
                waveIndices.push_back(i);
                break;
            case IrOpcode::PLACE_TOWER:
                placementIndices.push_back(i);
                break;
            default:
                break;
        }
    }

    // Generate enemies array
    if (!enemyIndices.empty()) {
        if (hasMap) json << ",\n";
        json << "    \"enemies\": [\n";

        for (size_t i = 0; i < enemyIndices.size(); i++) {
            json << generateEnemyJSON(instructions[enemyIndices[i]]);
            if (i + 1 < enemyIndices.size()) json << ",";
            json << "\n";
        }

        json << "    ]";
    }

    // Generate towers array
    if (!towerIndices.empty()) {
        if (hasMap || !enemyIndices.empty()) json << ",\n";
        json << "    \"towers\": [\n";

        for (size_t i = 0; i < towerIndices.size(); i++) {
            json << generateTowerJSON(instructions[towerIndices[i]]);
            if (i + 1 < towerIndices.size()) json << ",";
            json << "\n";
        }

        json << "    ]";
    }

    // Generate waves array
    if (!waveIndices.empty()) {
        if (hasMap || !enemyIndices.empty() || !towerIndices.empty()) json << ",\n";
        json << "    \"waves\": [\n";

        bool firstWave = true;
        for (size_t i = 0; i < instructions.size(); i++) {
            if (instructions[i].opcode == IrOpcode::DEFINE_WAVE) {
                if (!firstWave) json << ",\n";
                firstWave = false;
                json << generateWaveJSON(instructions, i);
            }
        }

        json << "    ]";
    }

    // Generate placements array
    if (!placementIndices.empty()) {
        if (hasMap || !enemyIndices.empty() || !towerIndices.empty() || !waveIndices.empty()) {
            json << ",\n";
        }
        json << "    \"initialPlacements\": [\n";

        for (size_t i = 0; i < placementIndices.size(); i++) {
            json << generatePlacementJSON(instructions[placementIndices[i]]);
            if (i + 1 < placementIndices.size()) json << ",";
            json << "\n";
        }

        json << "    ]";
    }

    json << "\n  }\n";
    json << "}\n";

    return json.str();
}

std::string CodeGenerator::generateReadable(const std::vector<IrInstruction>& instructions) {
    IrGenerator irGenerator;
    std::vector<std::string> lines = irGenerator.toString(instructions);

    std::ostringstream result;
    result << "=== TDLang Compiled Output ===\n\n";

    for (const auto& line : lines) {
        result << line << "\n";
    }

    return result.str();
}
