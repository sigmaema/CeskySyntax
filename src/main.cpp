#include "CodeGenerator.h"
#include "Executor.h"
#include "Lexer.h"
#include "Translator.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

bool startsWith(const std::string& text, const std::string& prefix) {
    if (text.size() < prefix.size()) {
        return false;
    }

    for (std::size_t i = 0; i < prefix.size(); ++i) {
        if (text[i] != prefix[i]) {
            return false;
        }
    }

    return true;
}

std::string readFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        return {};
    }

    std::ostringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

void printUsage(const char* programName) {
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " <input.custom> [output.cpp] [--run] [--compiler=<name>]\n\n";
    std::cout << "Example:\n";
    std::cout << "  " << programName << " examples/demo.csx generated.cpp --run --compiler=g++\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    const std::string inputPath = argv[1];
    std::string outputPath = "generated.cpp";
    bool runAfterGeneration = false;
    std::string compiler = "g++";

    if (argc >= 3 && !startsWith(std::string(argv[2]), "--")) {
        outputPath = argv[2];
    }

    for (int i = 2; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--run") {
            runAfterGeneration = true;
        } else if (startsWith(arg, "--compiler=")) {
            compiler = arg.substr(std::string("--compiler=").size());
        }
    }

    const std::string sourceCode = readFile(inputPath);
    if (sourceCode.empty()) {
        std::cerr << "Failed to read source file: " << inputPath << "\n";
        return 1;
    }

    const std::vector<std::string> lines = csx::Lexer::splitLines(sourceCode);
    csx::Translator translator;
    const std::vector<std::string> translated = translator.translateProgram(lines);
    const std::string cppCode = csx::CodeGenerator::generateCpp(translated);

    if (!csx::CodeGenerator::writeToFile(outputPath, cppCode)) {
        std::cerr << "Failed to write generated C++ file: " << outputPath << "\n";
        return 1;
    }

    std::cout << "Generated C++ file: " << outputPath << "\n";

    if (!runAfterGeneration) {
        return 0;
    }

    std::filesystem::path exePath = std::filesystem::path(outputPath).stem();
#ifdef _WIN32
    exePath += ".exe";
#endif

    if (!csx::Executor::compileAndRun(outputPath, exePath.string(), compiler)) {
        std::cerr << "Compilation or execution failed.\n";
        return 1;
    }

    return 0;
}
