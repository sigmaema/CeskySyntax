#include "Lexer.h"

#include <cctype>
#include <sstream>

namespace csx {

std::vector<std::string> Lexer::splitLines(const std::string& source) {
    std::vector<std::string> lines;
    std::stringstream stream(source);
    std::string line;

    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    return lines;
}

std::vector<std::string> Lexer::tokenizeWords(const std::string& line) {
    std::vector<std::string> tokens;
    std::string current;

    for (char ch : line) {
        if (std::isalnum(static_cast<unsigned char>(ch)) || ch == '_') {
            current.push_back(ch);
        } else {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            if (!std::isspace(static_cast<unsigned char>(ch))) {
                std::string singleChar;
                singleChar.push_back(ch);
                tokens.push_back(singleChar);
            }
        }
    }

    if (!current.empty()) {
        tokens.push_back(current);
    }

    return tokens;
}

}  // namespace csx
