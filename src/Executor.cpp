#include "Executor.h"

#include <cstdlib>

namespace csx {

bool Executor::compileAndRun(const std::string& cppPath, const std::string& executablePath, const std::string& compiler) {
    const std::string compileCmd = compiler + " \"" + cppPath + "\" -std=c++17 -O2 -o \"" + executablePath + "\"";
    if (std::system(compileCmd.c_str()) != 0) {
        return false;
    }

#ifdef _WIN32
    const std::string runCmd = std::string("\"") + executablePath + "\"";
#else
    const std::string runCmd = std::string("./\"") + executablePath + "\"";
#endif

    return std::system(runCmd.c_str()) == 0;
}

}  // namespace csx
