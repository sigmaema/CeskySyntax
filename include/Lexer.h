#pragma once

#include <string>
#include <vector>

namespace csx {

class Lexer {
public:
    static std::vector<std::string> splitLines(const std::string& source);
    static std::vector<std::string> tokenizeWords(const std::string& line);
};

}  // namespace csx
