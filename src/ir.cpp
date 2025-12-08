#include "mtdl/ir.hpp"
#include <sstream>

std::vector<IrInstruction> IrGenerator::generate(std::shared_ptr<Program> program) {
    code.clear(); // Clear any previous IR code

    for (const auto& declaration : program->declarations) {
        if (auto mapDecl = dynamic_cast<MapDecl*>(declaration.get())) {
            IrInstruction instruction(IrOpcode::DEFINE_MAP);
            instruction.operands.push_back(mapDecl->name);
            instruction.metadata["width"] = mapDecl->width;
            instruction.metadata["height"] = mapDecl->height;

            // Convert path to string format for metadata
            std::stringstream pathStream;
            for (size_t i = 0; i < mapDecl->path.size(); i++) {
                pathStream << mapDecl->path[i].first << "," << mapDecl->path[i].second;
                if (i + 1 < mapDecl->path.size()) pathStream << ";";
            }
            instruction.metadata["path"] = pathStream.str();
            emit(instruction);
        }
        else if (auto enemyDecl = dynamic_cast<EnemyDecl*>(declaration.get())) {
            IrInstruction instruction(IrOpcode::DEFINE_ENEMY);
            instruction.operands.push_back(enemyDecl->name);
            instruction.metadata["hp"] = enemyDecl->hp;
            instruction.metadata["speed"] = enemyDecl->speed;
            instruction.metadata["reward"] = enemyDecl->reward;
            emit(instruction);
        }
        else if (auto towerDecl = dynamic_cast<TowerDecl*>(declaration.get())) {
            IrInstruction instruction(IrOpcode::DEFINE_TOWER);
            instruction.operands.push_back(towerDecl->name);
            instruction.metadata["range"] = towerDecl->range;
            instruction.metadata["damage"] = towerDecl->damage;
            instruction.metadata["fire_rate"] = towerDecl->fireRate;
            instruction.metadata["cost"] = towerDecl->cost;
            emit(instruction);
        }
        else if (auto waveDecl = dynamic_cast<WaveDecl*>(declaration.get())) {
            // Define the wave
            IrInstruction instruction(IrOpcode::DEFINE_WAVE);
            instruction.operands.push_back(waveDecl->name);
            emit(instruction);

            // Add spawn instructions for this wave
            for (const auto& spawn : waveDecl->spawns) {
                IrInstruction spawnInstruction(IrOpcode::SPAWN_ENEMY);
                spawnInstruction.operands.push_back(waveDecl->name);
                spawnInstruction.operands.push_back(spawn.enemyType);
                spawnInstruction.metadata["count"] = spawn.count;
                spawnInstruction.metadata["start"] = spawn.start;
                spawnInstruction.metadata["interval"] = spawn.interval;
                emit(spawnInstruction);
            }
        }
        else if (auto placeStmt = dynamic_cast<PlaceStmt*>(declaration.get())) {
            IrInstruction instruction(IrOpcode::PLACE_TOWER);
            instruction.operands.push_back(placeStmt->towerType);
            instruction.metadata["x"] = placeStmt->x;
            instruction.metadata["y"] = placeStmt->y;
            emit(instruction);
        }
    }

    return code;
}

std::vector<std::string> IrGenerator::toString(const std::vector<IrInstruction>& instructions) {
    std::vector<std::string> result;

    for (const auto& instruction : instructions) {
        std::stringstream stream;

        switch (instruction.opcode) {
            case IrOpcode::DEFINE_MAP:
                stream << "DEFINE_MAP " << instruction.operands[0];
                if (instruction.metadata.count("width"))
                    stream << " WIDTH=" << std::get<int>(instruction.metadata.at("width"));
                if (instruction.metadata.count("height"))
                    stream << " HEIGHT=" << std::get<int>(instruction.metadata.at("height"));
                if (instruction.metadata.count("path"))
                    stream << " PATH=[" << std::get<std::string>(instruction.metadata.at("path")) << "]";
                break;

            case IrOpcode::DEFINE_ENEMY:
                stream << "DEFINE_ENEMY " << instruction.operands[0];
                if (instruction.metadata.count("hp"))
                    stream << " HP=" << std::get<int>(instruction.metadata.at("hp"));
                if (instruction.metadata.count("speed"))
                    stream << " SPEED=" << std::get<double>(instruction.metadata.at("speed"));
                if (instruction.metadata.count("reward"))
                    stream << " REWARD=" << std::get<int>(instruction.metadata.at("reward"));
                break;

            case IrOpcode::DEFINE_TOWER:
                stream << "DEFINE_TOWER " << instruction.operands[0];
                if (instruction.metadata.count("range"))
                    stream << " RANGE=" << std::get<int>(instruction.metadata.at("range"));
                if (instruction.metadata.count("damage"))
                    stream << " DAMAGE=" << std::get<int>(instruction.metadata.at("damage"));
                if (instruction.metadata.count("fire_rate"))
                    stream << " FIRERATE=" << std::get<double>(instruction.metadata.at("fire_rate"));
                if (instruction.metadata.count("cost"))
                    stream << " COST=" << std::get<int>(instruction.metadata.at("cost"));
                break;

            case IrOpcode::DEFINE_WAVE:
                stream << "DEFINE_WAVE " << instruction.operands[0];
                break;

            case IrOpcode::SPAWN_ENEMY:
                stream << "  SPAWN_ENEMY " << instruction.operands[1] << " IN_WAVE=" << instruction.operands[0];
                if (instruction.metadata.count("count"))
                    stream << " COUNT=" << std::get<int>(instruction.metadata.at("count"));
                if (instruction.metadata.count("start"))
                    stream << " START=" << std::get<int>(instruction.metadata.at("start"));
                if (instruction.metadata.count("interval"))
                    stream << " INTERVAL=" << std::get<int>(instruction.metadata.at("interval"));
                break;

            case IrOpcode::PLACE_TOWER:
                stream << "PLACE_TOWER " << instruction.operands[0];
                if (instruction.metadata.count("x"))
                    stream << " X=" << std::get<int>(instruction.metadata.at("x"));
                if (instruction.metadata.count("y"))
                    stream << " Y=" << std::get<int>(instruction.metadata.at("y"));
                break;

            case IrOpcode::NOP:
                stream << "NOP";
                break;

            default:
                stream << "UNKNOWN_OPCODE";
        }

        result.push_back(stream.str());
    }

    return result;
}
