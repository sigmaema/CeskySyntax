// Dear ImGui GUI editor implementation.
// Requires: GLFW + OpenGL3 + Dear ImGui (provide via vcpkg or your system libs).
// Build with CMake: -DBUILD_GUI=ON and ensure `glfw` and `imgui` targets are available.

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "ImGuiSimpleEditor.h"
#ifdef CESKYSYNTAX_HAS_TEXTEDITOR
#include "TextEditor.h"
#endif

// Minimal process capture helper (Windows and POSIX fallback)
static std::string runCommandCapture(const std::string& command) {
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
    std::string mutableCmd = command;
    BOOL started = CreateProcessA(nullptr, mutableCmd.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &startupInfo, &processInfo);
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
    if (!pipe) return "Chyba: Nelze spustit příkaz.\n";
    std::string output;
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), pipe)) output += buffer;
    pclose(pipe);
    return output;
#endif
}

static std::string getProjectRoot() {
    char cwd[1024];
#ifdef _WIN32
    if (_getcwd(cwd, sizeof(cwd)) == nullptr) return std::string();
#else
    if (getcwd(cwd, sizeof(cwd)) == nullptr) return std::string();
#endif
    return std::string(cwd);
}

static std::string loadFileToString(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return std::string();
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static bool saveStringToFile(const std::string& path, const std::string& content) {
    std::ofstream f(path, std::ios::binary);
    if (!f) return false;
    f << content;
    return true;
}

static std::string runTranspileAndRun(const std::string& sourceText) {
    const std::string projectRoot = getProjectRoot();
    if (projectRoot.empty()) return "Chyba: Nelze určit projektový adresář.\n";
    const std::string srcPath = projectRoot + "\\temp_source.csx";
    const std::string outCpp = projectRoot + "\\temp_generated.cpp";
    const std::string transpiler = projectRoot + "\\build\\cesky_transpiler.exe";

    std::ofstream f(srcPath, std::ios::binary);
    if (!f) return "Chyba: Nelze zapsat zdrojový soubor.\n";
    f << sourceText;
    f.close();

    std::ostringstream cmd;
    cmd << '"' << transpiler << '"' << " \"" << srcPath << "\" \"" << outCpp << "\" --run --compiler=g++.exe";
    return runCommandCapture(cmd.str());
}

#ifdef CESKYSYNTAX_HAS_TEXTEDITOR
static void updateDeclaredVariables(const std::string& code, TextEditor::LanguageDefinition& langDef) {
    langDef.mIdentifiers.clear();
    
    // Pattern to match: (type keyword) (variable name)
    // Examples: "celé číslo x", "levituj foo", "provázek myVar", etc.
    std::regex typePattern(R"((celé\s+číslo|levituj|dvojté|charakter|provázek|pravdivostní\s+hodnota)\s+([a-zA-Z_][a-zA-Z0-9_]*))");
    std::smatch match;
    std::string searchText = code;
    
    while (std::regex_search(searchText, match, typePattern)) {
        std::string varName = match[2].str();
        langDef.mIdentifiers[varName] = TextEditor::Identifier();
        searchText = match.suffix().str();
    }
}

static TextEditor::LanguageDefinition createCzechLanguageDefinition() {
    TextEditor::LanguageDefinition langDef;
    langDef.mName = "CeskySyntax";
    langDef.mCaseSensitive = true;
    langDef.mAutoIndentation = true;
    langDef.mCommentStart = "/*";
    langDef.mCommentEnd = "*/";
    langDef.mSingleLineComment = "//";

    const char* const keywords[] = {
        "celé číslo",
        "levituj",
        "dvojté",
        "charakter",
        "pravdivostní hodnota",
        "prázdnota",
        "provázek",
        "pravda",
        "falešný",
        "když",
        "jinak kdyby",
        "jinak",
        "přepínač",
        "věc",
        "výchozí",
        "přerušení",
        "pokračovat",
        "návrat",
        "pro",
        "zatímco",
        "dělat",
        "třída",
        "strukturovat",
        "veřejnost",
        "soukromé",
        "chráněný",
        "tento",
        "viruální",
        "přepsat",
        "nový",
        "smazat",
        "v řadě za sebou",
        "jmenný prostor",
        "pomocí",
        "pokus",
        "chytit",
        "hod",
        "nulový ukazatel",
        "norma",
        "norma::znakový výstup",
        "znakový výstup",
        "zadávání znaků",
        "norma::koncová čára",
        "koncová čára",
        "rovná se",
        "plus",
        "mínus",
        "krát",
        "děleno",
        "zbytek po dělení",
        "rovná se?",
        "nerovná se",
        "menší než",
        "větší než",
        "menší nebo rovno",
        "větší nebo rovno",
        "a zároveň",
        "nebo",
        "není"
    };
    for (const char* keyword : keywords) {
        langDef.mKeywords.insert(keyword);
    }

    langDef.mTokenRegexStrings.push_back({R"(norma::znakový\s+výstup)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(znakový\s+výstup)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(norma::koncová\s+čára)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(koncová\s+čára)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(jinak\s+kdyby)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(v\s+řadě\s+za\s+sebou)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(jmenný\s+prostor)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(nulový\s+ukazatel)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(pravdivostní\s+hodnota)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(celé\s+číslo)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(rovná\s+se\?)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(rovná\s+se)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(nerovná\s+se)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(menší\s+než)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(větší\s+než)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(menší\s+nebo\s+rovno)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(větší\s+nebo\s+rovno)", TextEditor::PaletteIndex::Keyword});
    langDef.mTokenRegexStrings.push_back({R"(a\s+zároveň)", TextEditor::PaletteIndex::Keyword});

    langDef.mTokenRegexStrings.push_back({R"(L?"(\\.|[^"])*")", TextEditor::PaletteIndex::String});
    langDef.mTokenRegexStrings.push_back({R"(\'\\?[^']\')", TextEditor::PaletteIndex::CharLiteral});
    langDef.mTokenRegexStrings.push_back({R"([+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?)", TextEditor::PaletteIndex::Number});
    langDef.mTokenRegexStrings.push_back({R"([+-]?[0-9]+[Uu]?[lL]?[lL]?)", TextEditor::PaletteIndex::Number});
    langDef.mTokenRegexStrings.push_back({R"(0[0-7]+[Uu]?[lL]?[lL]?)", TextEditor::PaletteIndex::Number});
    langDef.mTokenRegexStrings.push_back({R"(0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?)", TextEditor::PaletteIndex::Number});
    langDef.mTokenRegexStrings.push_back({R"([a-zA-Z_][a-zA-Z0-9_]*)", TextEditor::PaletteIndex::Identifier});
    langDef.mTokenRegexStrings.push_back({R"([\[\]\{\}\!\%\^\&\*\(\)\-\+\=\~\|\<\>\?\/\;\,\.])", TextEditor::PaletteIndex::Punctuation});

    return langDef;
}

static TextEditor::Palette createCzechPalette() {
    TextEditor::Palette palette = TextEditor::GetDarkPalette();
    palette[(int)TextEditor::PaletteIndex::Keyword] = 0xfff0c674;
    palette[(int)TextEditor::PaletteIndex::String] = 0xff98c379;
    palette[(int)TextEditor::PaletteIndex::Number] = 0xff61afef;
    palette[(int)TextEditor::PaletteIndex::Comment] = 0xff7f848e;
    palette[(int)TextEditor::PaletteIndex::LineNumber] = 0xff7d8590;
    palette[(int)TextEditor::PaletteIndex::Background] = 0xff000000;
    palette[(int)TextEditor::PaletteIndex::Selection] = 0x807aa2f7;
    palette[(int)TextEditor::PaletteIndex::CurrentLineFill] = 0x404b5563;
    palette[(int)TextEditor::PaletteIndex::CurrentLineEdge] = 0x406b7280;
    // Green color for variable declarations (KnownIdentifier)
    palette[(int)TextEditor::PaletteIndex::KnownIdentifier] = 0xff00ff00;
    return palette;
}
#endif

static void loadCzechCapableFont(ImGuiIO& io) {
    const ImWchar* glyphRanges = io.Fonts->GetGlyphRangesDefault();

#ifdef _WIN32
    const char* candidates[] = {
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };

    for (const char* path : candidates) {
        std::ifstream f(path, std::ios::binary);
        if (!f.good()) {
            continue;
        }
        ImFont* font = io.Fonts->AddFontFromFileTTF(path, 24.0f, nullptr, glyphRanges);
        if (font != nullptr) {
            io.FontDefault = font;
            return;
        }
    }
#endif

    // Fallback for non-Windows or missing system fonts.
    io.FontDefault = io.Fonts->AddFontDefault();
}

int main(int, char**)
{
    // Setup GLFW
    if (!glfwInit()) return -1;
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1600, 900, "CeskySyntax GUI Editor", nullptr, nullptr);
    if (window == nullptr) return -1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    loadCzechCapableFont(io);

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.FrameRounding = 7.0f;
    style.GrabRounding = 7.0f;
    style.ScrollbarRounding = 9.0f;
    style.WindowPadding = ImVec2(14.0f, 12.0f);
    style.FramePadding = ImVec2(10.0f, 7.0f);
    style.ItemSpacing = ImVec2(10.0f, 8.0f);
    style.ScrollbarSize = 18.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.30f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.40f, 0.63f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.24f, 0.34f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.35f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.16f, 0.43f, 0.72f, 1.00f);
    style.ScaleAllSizes(1.15f);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Editor buffers
    static std::string code = "celé číslo x = 0\n\nzatímco x < 3 {\n    norma::znakový výstup << x << norma::koncová čára\n    x = x plus 1\n}\n";
    static std::string output;
    static ImGuiSimpleEditor simpleEditor;
#ifdef CESKYSYNTAX_HAS_TEXTEDITOR
    static TextEditor advancedEditor;
    static bool advancedEditorInitialized = false;
#endif

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main editor window (resizable)
        ImGui::SetNextWindowSize(ImVec2(1600.0f, 900.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("Editor kódu", nullptr, ImGuiWindowFlags_None);
        ImGui::Text("CeskySyntax - grafický editor");

    #ifdef CESKYSYNTAX_HAS_TEXTEDITOR
        if (!advancedEditorInitialized) {
            auto langDef = createCzechLanguageDefinition();
            updateDeclaredVariables(code, langDef);
            advancedEditor.SetLanguageDefinition(langDef);
            advancedEditor.SetPalette(createCzechPalette());
            advancedEditor.SetShowWhitespaces(false);
            advancedEditor.SetText(code);
            advancedEditorInitialized = true;
        }
    #endif

        // Toolbar
        if (ImGui::Button("Načíst příklad kódu")) {
            const std::string examplePath = getProjectRoot() + "\\examples\\smoke-current.csx";
            std::string loaded = loadFileToString(examplePath);
            if (!loaded.empty()) {
                code.swap(loaded);
#ifdef CESKYSYNTAX_HAS_TEXTEDITOR
                advancedEditor.SetText(code);
                auto langDef = createCzechLanguageDefinition();
                updateDeclaredVariables(code, langDef);
                advancedEditor.SetLanguageDefinition(langDef);
#endif
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Uložit")) {
#ifdef CESKYSYNTAX_HAS_TEXTEDITOR
            code = advancedEditor.GetText();
#endif
            const std::string examplePath = getProjectRoot() + "\\examples\\smoke-current.csx";
            saveStringToFile(examplePath, code);
        }
        ImGui::SameLine();
        if (ImGui::Button("Přeložit a spustit")) {
#ifdef CESKYSYNTAX_HAS_TEXTEDITOR
            code = advancedEditor.GetText();
#endif
            output = runTranspileAndRun(code);
        }
        ImGui::SameLine();
        if (ImGui::Button("Spustit poslední spustitelný kód")) {
            const std::string exePath = getProjectRoot() + "\\temp_generated.exe";
            std::ostringstream cmd; cmd << '"' << exePath << '"';
            output = runCommandCapture(cmd.str());
        }

        ImGui::Separator();

#ifdef CESKYSYNTAX_HAS_TEXTEDITOR
    auto cpos = advancedEditor.GetCursorPosition();
    ImGui::Text("Řádek %d, Sloupec %d | %s | %s", cpos.mLine + 1, cpos.mColumn + 1,
        advancedEditor.CanUndo() ? "Lze vrátit zpět" : "Nelze vrátit zpět",
        advancedEditor.IsOverwrite() ? "OVR" : "INS");
    ImGui::Separator();
#endif

        // Update declared variables if text changed (for live highlighting)
#ifdef CESKYSYNTAX_HAS_TEXTEDITOR
        static std::string lastEditorText = code;
        std::string currentEditorText = advancedEditor.GetText();
        if (currentEditorText != lastEditorText) {
            lastEditorText = currentEditorText;
            auto langDef = advancedEditor.GetLanguageDefinition();
            updateDeclaredVariables(currentEditorText, const_cast<TextEditor::LanguageDefinition&>(langDef));
            advancedEditor.SetLanguageDefinition(langDef);
        }
#endif

        // Editor area (uses ImGuiSimpleEditor)
#ifdef CESKYSYNTAX_HAS_TEXTEDITOR
    advancedEditor.Render("CeskyEditorAdvanced", ImVec2(-1.0f, ImGui::GetContentRegionAvail().y - 120.0f), true);
#else
    simpleEditor.Render("CeskyEditor", code, ImVec2(0, ImGui::GetContentRegionAvail().y - 120));
#endif

        ImGui::End();

        // Output window (read-only, scrollable, resizable)
        ImGui::SetNextWindowSize(ImVec2(800.0f, 140.0f), ImGuiCond_FirstUseEver);
        ImGui::Begin("Výstup", nullptr, ImGuiWindowFlags_None);
        ImGui::BeginChild("VýstupScroll", ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(output.c_str());
        ImGui::EndChild();
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
