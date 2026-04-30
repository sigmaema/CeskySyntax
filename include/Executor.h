#pragma once

#include <string>

namespace csx {

class Executor {
public:
    static bool compileAndRun(const std::string& cppPath, const std::string& executablePath, const std::string& compiler = "g++");
};

}  // namespace csx
