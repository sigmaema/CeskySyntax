#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace csx {

class Translator {
public:
    Translator();

    std::vector<std::string> translateProgram(const std::vector<std::string>& lines) const;

private:
    std::unordered_map<std::string, std::string> keywordMap;

    std::string translateLine(const std::string& line) const;
    std::string replaceWholeWords(const std::string& text) const;
    static std::string ensureSemicolon(const std::string& line);
};

}  // namespace csx
