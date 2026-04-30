#pragma once

#include <string>
#include <vector>

namespace csx {

class CodeGenerator {
public:
    static std::string generateCpp(const std::vector<std::string>& translatedLines);
    static bool writeToFile(const std::string& outputPath, const std::string& cppCode);
};

}  // namespace csx
