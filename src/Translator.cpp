#include "Translator.h"

#include <algorithm>
#include <cctype>
#include <regex>

namespace {

std::string ltrim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return s;
}

std::string rtrim(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    return s;
}

std::string trim(std::string s) {
    return rtrim(ltrim(std::move(s)));
}

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

bool isIdentifierChar(unsigned char ch) {
    return std::isalnum(ch) != 0 || ch == '_';
}

bool hasBoundary(const std::string& text, std::size_t position, std::size_t length) {
    const bool leftBoundary = position == 0 || !isIdentifierChar(static_cast<unsigned char>(text[position - 1]));
    const std::size_t rightIndex = position + length;
    const bool rightBoundary = rightIndex >= text.size() || !isIdentifierChar(static_cast<unsigned char>(text[rightIndex]));
    return leftBoundary && rightBoundary;
}

std::string replaceWholePhrase(const std::string& text, const std::string& source, const std::string& target) {
    if (source.empty()) {
        return text;
    }

    std::string result;
    std::size_t searchPosition = 0;

    while (searchPosition < text.size()) {
        const std::size_t found = text.find(source, searchPosition);
        if (found == std::string::npos) {
            result.append(text.substr(searchPosition));
            break;
        }

        result.append(text.substr(searchPosition, found - searchPosition));

        if (hasBoundary(text, found, source.size())) {
            result.append(target);
        } else {
            result.append(source);
        }

        searchPosition = found + source.size();
    }

    return result;
}

std::string normalizeControlHeader(const std::string& line, const std::string& keyword) {
    const std::regex alreadyParenthesized(std::string("^") + keyword + "\\s*\\((.*)\\)\\s*\\{$");
    if (std::regex_match(line, alreadyParenthesized)) {
        return line;
    }

    const std::regex rawHeader(std::string("^") + keyword + "\\s+(.+)\\s*\\{$");
    std::smatch match;
    if (std::regex_match(line, match, rawHeader) && match.size() > 1) {
        return keyword + std::string(" (") + trim(match[1].str()) + ") {";
    }

    return line;
}

}  // namespace

namespace csx {

Translator::Translator()
    : keywordMap{
          {"celé číslo", "int"},
          {"levituj", "float"},
          {"krátké desetinné číslo", "float"},
          {"dvojté", "double"},
          {"dlouhé desetinné číslo", "double"},
          {"charakter", "char"},
          {"znak", "char"},
          {"pravdivostní hodnota", "bool"},
          {"prázdnota", "void"},
          {"funkce", "void"},
          {"provázek", "string"},
          {"text", "string"},
          {"pravda", "true"},
          {"falešný", "false"},
          {"nepravda", "false"},
          {"když", "if"},
          {"jinak kdyby", "else if"},
          {"jinak", "else"},
          {"přepínač", "switch"},
          {"věc", "case"},
          {"případ", "case"},
          {"výchozí", "default"},
          {"přerušení", "break"},
          {"pokračovat", "continue"},
          {"návrat", "return"},
          {"vrátit", "return"},
          {"pro", "for"},
          {"zatímco", "while"},
          {"dělat", "do"},
          {"třída", "class"},
          {"strukturovat", "struct"},
          {"veřejnost", "public"},
          {"veřejný", "public"},
          {"soukromý", "private"},
          {"chráněný", "protected"},
          {"tento", "this"},
          {"viruální", "virtual"},
          {"přepsat", "override"},
          {"nový", "new"},
          {"smazat", "delete"},
          {"v řadě za sebou", "inline"},
          {"jmenný prostor", "namespace"},
          {"pomocí", "using"},
          {"pokus", "try"},
          {"chytit", "catch"},
          {"hod", "throw"},
          {"nulový ukazatel", "nullptr"},
          {"norma", "std"},
          {"norma::znakový výstup", "std::cout"},
          {"začátek výpisu", "std::cout"},
          {"znakový výstup", "cout"},
          {"zadávání znaků", "cin"},
          {"norma::koncová čára", "std::endl"},
          {"konec výpisu", "std::endl"},
          {"koncová čára", "endl"},
          {"rovná se", "="},
          {"plus", "+"},
          {"mínus", "-"},
          {"krát", "*"},
          {"děleno", "/"},
          {"zbytek po dělení", "%"},
          {"rovná se?", "=="},
          {"nerovná se", "!="},
          {"menší než", "<"},
          {"větší než", ">"},
          {"menší nebo rovno", "<="},
          {"větší nebo rovno", ">="},
          {"a zároveň", "&&"},
          {"nebo", "||"},
          {"není", "!"},
      } {}

std::vector<std::string> Translator::translateProgram(const std::vector<std::string>& lines) const {
    std::vector<std::string> translated;
    translated.reserve(lines.size());

    for (const std::string& line : lines) {
        std::string out = translateLine(line);
        if (!out.empty()) {
            translated.push_back(out);
        }
    }

    return translated;
}

std::string Translator::replaceWholeWords(const std::string& text) const {
    std::string result = text;

    std::vector<std::pair<std::string, std::string>> replacements(keywordMap.begin(), keywordMap.end());
    std::sort(replacements.begin(), replacements.end(), [](const auto& left, const auto& right) {
        return left.first.size() > right.first.size();
    });

    for (const auto& [sourceWord, targetWord] : replacements) {
        result = replaceWholePhrase(result, sourceWord, targetWord);
    }

    return result;
}

std::string Translator::ensureSemicolon(const std::string& line) {
    std::string t = trim(line);
    if (t.empty()) {
        return t;
    }

    const char last = t.back();
    if (last == ';' || last == '{' || last == '}') {
        return t;
    }

    return t + ";";
}

std::string Translator::translateLine(const std::string& line) const {
    const std::string stripped = trim(line);

    if (stripped.empty() || startsWith(stripped, "#")) {
        return std::string();
    }

    if (startsWith(stripped, "vypis ")) {
        std::string expression = trim(stripped.substr(std::string("vypis").size()));
        expression = replaceWholeWords(expression);
        return std::string("std::cout << (") + expression + ") << std::endl;";
    }

    {
        const std::regex outputPattern(R"(^začátek\s+výpisu\s*<<\s*(.*?)\s*<<\s*konec\s+výpisu\s*$)");
        std::smatch outputMatch;
        if (std::regex_match(stripped, outputMatch, outputPattern) && outputMatch.size() > 1) {
            std::string expression = replaceWholeWords(trim(outputMatch[1].str()));
            return std::string("std::cout << (") + expression + ") << std::endl;";
        }
    }

    std::string replaced = replaceWholeWords(stripped);

    replaced = normalizeControlHeader(replaced, "if");
    replaced = normalizeControlHeader(replaced, "while");
    replaced = normalizeControlHeader(replaced, "for");

    return ensureSemicolon(replaced);
}

}  // namespace csx
