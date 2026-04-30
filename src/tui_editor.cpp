#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
    #include <direct.h>
    #include <windows.h>
    #define GET_CWD _getcwd
#else
    #include <unistd.h>
    #define GET_CWD getcwd
#endif

namespace {

std::string trim(const std::string& text) {
    const std::size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }

    const std::size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

void setEnvVar(const std::string& name, const std::string& value) {
#ifdef _WIN32
    _putenv_s(name.c_str(), value.c_str());
#else
    setenv(name.c_str(), value.c_str(), 1);
#endif
}

void loadEnvFile(const std::string& projectRoot) {
    const std::string envPath = projectRoot + "\\.env";
    std::ifstream envFile(envPath);
    if (!envFile) {
        return;
    }

    std::string line;
    while (std::getline(envFile, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const std::size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, equalsPos));
        std::string value = trim(line.substr(equalsPos + 1));
        if (!key.empty() && !value.empty()) {
            setEnvVar(key, value);
        }
    }
}

std::string getEnvOrDefault(const char* name, const std::string& fallback) {
    const char* value = std::getenv(name);
    if (value != nullptr && *value != '\0') {
        return value;
    }
    return fallback;
}

std::string runCommandCapture(const std::string& command) {
#ifdef _WIN32
    SECURITY_ATTRIBUTES securityAttributes{};
    securityAttributes.nLength = sizeof(securityAttributes);
    securityAttributes.bInheritHandle = TRUE;

    HANDLE readPipe = nullptr;
    HANDLE writePipe = nullptr;
    if (!CreatePipe(&readPipe, &writePipe, &securityAttributes, 0)) {
        return "Chyba: Nelze vytvořit potrubí procesu.\n";
    }

    if (!SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(readPipe);
        CloseHandle(writePipe);
        return "Chyba: Nelze konfigurovat potrubí procesu.\n";
    }

    STARTUPINFOA startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESTDHANDLES;
    startupInfo.hStdOutput = writePipe;
    startupInfo.hStdError = writePipe;
    startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION processInfo{};
    std::string mutableCommand = command;
    BOOL started = CreateProcessA(
        nullptr,
        mutableCommand.data(),
        nullptr,
        nullptr,
        TRUE,
        0,
        nullptr,
        nullptr,
        &startupInfo,
        &processInfo);

    CloseHandle(writePipe);

    if (!started) {
        CloseHandle(readPipe);
        return "Chyba: Nelze spustit příkaz.\n";
    }

    std::string output;
    char buffer[512];
    DWORD bytesRead = 0;
    while (ReadFile(readPipe, buffer, static_cast<DWORD>(sizeof(buffer)), &bytesRead, nullptr) && bytesRead > 0) {
        output.append(buffer, bytesRead);
    }

    WaitForSingleObject(processInfo.hProcess, INFINITE);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
    CloseHandle(readPipe);
    return output;
#else
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "Chyba: Nelze spustit příkaz.\n";
    }

    std::string output;
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }
    pclose(pipe);
    return output;
#endif
}

void prependToPath(const std::string& directory) {
    if (directory.empty()) {
        return;
    }

    const std::string currentPath = getEnvOrDefault("PATH", "");
    if (currentPath.empty()) {
        setEnvVar("PATH", directory);
    } else {
        setEnvVar("PATH", directory + ";" + currentPath);
    }
}

std::string getProjectRoot() {
    char cwd[1024];
    if (GET_CWD(cwd, sizeof(cwd)) == nullptr) {
        return "";
    }
    return std::string(cwd);
}

const std::string TRANSPILER_PATH = "build/cesky_transpiler.exe";
const std::string SOURCE_FILE = "temp_source.csx";
const std::string OUTPUT_FILE = "temp_generated.cpp";
const std::string EXE_FILE = "temp_generated.exe";
const std::string COMPILER = "g++.exe";

std::string runTranspiler(const std::vector<std::string>& lines) {
    const std::string projectRoot = getProjectRoot();
    if (projectRoot.empty()) {
        return "Chyba: Nelze určit adresář projektu.\n";
    }

    const std::string sourcePath = projectRoot + "\\temp_source.csx";
    const std::string outputPath = projectRoot + "\\temp_generated.cpp";
    const std::string transpilerPath = projectRoot + "\\build\\cesky_transpiler.exe";
    const std::string compilerPath = getEnvOrDefault("CESKYSYNTAX_COMPILER", "g++.exe");
    const std::string toolBinPath = getEnvOrDefault("CESKYSYNTAX_TOOLBIN", "");

    prependToPath(toolBinPath);

    std::ofstream srcFile(sourcePath, std::ios::binary);
    if (!srcFile) {
        return "Chyba: Nelze zapsat zdrojový soubor.\n";
    }

    for (const auto& line : lines) {
        srcFile << line << "\n";
    }
    srcFile.close();

    std::ostringstream cmd;
    cmd << '"' << transpilerPath << '"'
        << " \"" << sourcePath << "\""
        << " \"" << outputPath << "\""
        << " --run --compiler=\"" << compilerPath << "\"";

    return runCommandCapture(cmd.str());
}

std::string runGeneratedExe() {
    const std::string projectRoot = getProjectRoot();
    if (projectRoot.empty()) {
        return "Chyba: Nelze určit adresář projektu.\n";
    }

    const std::string exePath = projectRoot + "\\temp_generated.exe";
    
    // Check if exe exists
    FILE* checkFile = fopen(exePath.c_str(), "r");
    if (!checkFile) {
        return "Chyba: temp_generated.exe nebyl nalezen. Prosím spusťte nejprve příkaz 'spustit' pro kompilaci.\n";
    }
    fclose(checkFile);

    std::ostringstream cmd;
    cmd << '"' << exePath << '"';
    return runCommandCapture(cmd.str());
}

void displayCode(const std::vector<std::string>& codeLines) {
    std::cout << "\n--- Kód ---\n";
    for (std::size_t i = 0; i < codeLines.size(); ++i) {
        std::cout << i << ": " << codeLines[i] << "\n";
    }
    std::cout << "--- Konec ---\n\n";
}

}  // namespace

int main() {
    // Set console to UTF-8 mode on Windows
    #ifdef _WIN32
        system("chcp 65001 > nul 2>&1");
    #endif

    loadEnvFile(getProjectRoot());

    std::vector<std::string> codeLines;
    std::string output;
    bool quit = false;

    std::cout << "CeskySyntax TUI Editor\n";
    std::cout << "Příkazy: pridat, upravit <N>, smazat <N>, ukazat, spustit, spustitexe, smazatvse, ukoncit\n\n";

    while (!quit) {
        std::cout << "> ";
        std::string cmdLine;
        std::getline(std::cin, cmdLine);

        if (cmdLine.empty()) {
            continue;
        }

        std::istringstream iss(cmdLine);
        std::string cmd;
        iss >> cmd;

        if (cmd == "pridat") {
            std::string line;
            std::cout << "Zadejte řádky (prázdný řádek pro ukončení):\n";
            while (true) {
                std::cout << "  >>> ";
                std::getline(std::cin, line);
                if (line.empty()) {
                    break;
                }
                codeLines.push_back(line);
                std::cout << "  Řádek " << (codeLines.size() - 1) << " přidán.\n";
            }
            displayCode(codeLines);
        } else if (cmd == "upravit") {
            int lineNum;
            iss >> lineNum;
            if (lineNum >= 0 && lineNum < static_cast<int>(codeLines.size())) {
                std::cout << "Aktuální: " << codeLines[lineNum] << "\n";
                std::cout << "Nový řádek: ";
                std::string newLine;
                std::getline(std::cin, newLine);
                codeLines[lineNum].assign(newLine);
                std::cout << "Řádek byl aktualizován.\n";
                displayCode(codeLines);
            } else {
                std::cout << "Neplatné číslo řádku.\n";
            }
        } else if (cmd == "smazat") {
            int lineNum;
            iss >> lineNum;
            if (lineNum >= 0 && lineNum < static_cast<int>(codeLines.size())) {
                codeLines.erase(codeLines.begin() + lineNum);
                std::cout << "Řádek byl smazán.\n";
                displayCode(codeLines);
            } else {
                std::cout << "Neplatné číslo řádku.\n";
            }
        } else if (cmd == "ukazat") {
            displayCode(codeLines);
        } else if (cmd == "spustit") {
            std::cout << "\nSpouštím...\n";
            output = runTranspiler(codeLines);
            std::cout << output << "\n";
        } else if (cmd == "spustitexe") {
            std::cout << "\nSpouštění vygenerovaného souboru...\n";
            output = runGeneratedExe();
            std::cout << output << "\n";
        } else if (cmd == "smazatvse") {
            codeLines.clear();
            output.clear();
            std::cout << "Kód byl vymazán.\n";
            displayCode(codeLines);
        } else if (cmd == "ukoncit" || cmd == "u") {
            quit = true;
            std::cout << "Nashledanou\n";
        } else if (cmd == "kruh"){
            std::cout << "Nahhhh, polygon\n";
        } else {
            std::cout << "Neznámý příkaz: " << cmd << "\n";
        }
    }

    return 0;
}
