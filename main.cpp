#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "ir.h"
#include "optimizer.h"
#include "codegen.h"

// Utility function to read entire file into string
std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Debug function to dump IR instructions
void dumpIR(const std::vector<IrInstruction>& instructions) {
    std::cerr << "---- IR Dump ----\n";
    for (size_t i = 0; i < instructions.size(); ++i) {
        const auto &instruction = instructions[i];
        std::cerr << i << ": opcode=" << static_cast<int>(instruction.opcode)
                  << ", operands=[";
        for (size_t k = 0; k < instruction.operands.size(); ++k) {
            if (k) std::cerr << ", ";
            std::cerr << '"' << instruction.operands[k] << '"';
        }
        std::cerr << "]";
        if (!instruction.metadata.empty())
            std::cerr << ", metadata_count=" << instruction.metadata.size();
        std::cerr << "\n";
    }
    std::cerr << "-----------------\n";
}

// Utility function to write output to file
void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not write to file " << filename << std::endl;
        exit(1);
    }
    file << content;
    file.close();
}

// Print command-line usage information
void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " <input_file> [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -o <file>     Output file (default: output.json)\n";
    std::cout << "  -ir           Output IR to stdout\n";
    std::cout << "  -readable     Output readable format instead of JSON\n";
    std::cout << "  -no-opt       Disable optimization\n";
    std::cout << "  -h, --help    Show this help message\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    // Parse command line arguments
    std::string inputFile = argv[1];
    std::string outputFile = "output.json";
    bool showIR = false;
    bool readableFormat = false;
    bool optimize = true;

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-ir") {
            showIR = true;
        } else if (arg == "-readable") {
            readableFormat = true;
        } else if (arg == "-no-opt") {
            optimize = false;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    std::cout << "=== TDLang Compiler ===\n";
    std::cout << "Input: " << inputFile << "\n\n";

    // Phase 1: Lexical Analysis
    std::cout << "[Phase 1] Lexical Analysis...\n";
    std::string source = readFile(inputFile);
    Lexer lexer(source);

    // Phase 2: Syntax Analysis (Parsing)
    std::cout << "[Phase 2] Syntax Analysis (Parsing)...\n";
    Parser parser(lexer);
    std::shared_ptr<Program> ast;

    try {
        ast = parser.parseProgram();
        std::cout << "  Parsing successful.\n";
    } catch (const std::exception& error) {
        std::cerr << "  Parse error: " << error.what() << std::endl;
        return 1;
    }

    // Phase 3: Semantic Analysis
    std::cout << "[Phase 3] Semantic Analysis...\n";
    SemanticAnalyzer analyzer;

    try {
        analyzer.analyze(ast);
        std::cout << "  Semantic analysis passed.\n";
    } catch (const std::exception& error) {
        std::cerr << "  Semantic error: " << error.what() << std::endl;
        return 1;
    }

    // Phase 4: Intermediate Code Generation
    std::cout << "[Phase 4] Intermediate Code Generation...\n";
    IrGenerator irGenerator;
    std::vector<IrInstruction> ir = irGenerator.generate(ast);
    std::cout << "  Generated " << ir.size() << " IR instructions.\n";

    if (showIR) {
        std::cout << "\n--- Unoptimized IR ---\n";
        auto irLines = irGenerator.toString(ir);
        for (const auto& line : irLines) {
            std::cout << line << "\n";
        }
    }

    // Phase 5: Optimization
    std::vector<IrInstruction> optimizedIR = ir;

    if (optimize) {
        std::cout << "[Phase 5] Optimization...\n";
        Optimizer optimizer;
        optimizedIR = optimizer.optimize(ir);
        std::cout << "  Optimized to " << optimizedIR.size() << " instructions.\n";

        if (showIR) {
            std::cout << "\n--- Optimized IR ---\n";
            auto optLines = irGenerator.toString(optimizedIR);
            for (const auto& line : optLines) {
                std::cout << line << "\n";
            }
        }
    } else {
        std::cout << "[Phase 5] Optimization (skipped)\n";
    }

    // Phase 6: Code Generation
    std::cout << "[Phase 6] Code Generation...\n";
    CodeGenerator codeGenerator;

    dumpIR(optimizedIR);

    std::string output;

    if (readableFormat) {
        output = codeGenerator.generateReadable(optimizedIR);
    } else {
        output = codeGenerator.generateJSON(optimizedIR);
    }

    // Write output to file
    writeFile(outputFile, output);
    std::cout << "  Code generation complete.\n";
    std::cout << "\n=== Compilation Successful ===\n";
    std::cout << "Output written to: " << outputFile << "\n";

    return 0;
}
