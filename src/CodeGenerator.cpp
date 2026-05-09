#include "CodeGenerator.h"

#include <fstream>
#include <sstream>

namespace csx {

std::string CodeGenerator::generateCpp(const std::vector<std::string>& translatedLines, bool wrapInMain) {
    std::ostringstream output;

    output << "#include <iostream>\n";
    output << "#include <string>\n\n";

    if (wrapInMain) {
        output << "int main() {\n";

        for (const std::string& line : translatedLines) {
            output << "    " << line << "\n";
        }

        output << "    return 0;\n";
        output << "}\n";
    } else {
        for (const std::string& line : translatedLines) {
            output << line << "\n";
        }
    }

    return output.str();
}

bool CodeGenerator::writeToFile(const std::string& outputPath, const std::string& cppCode) {
    std::ofstream outFile(outputPath, std::ios::out | std::ios::trunc);
    if (!outFile) {
        return false;
    }

    outFile << cppCode;
    return true;
}

}  // namespace csx
