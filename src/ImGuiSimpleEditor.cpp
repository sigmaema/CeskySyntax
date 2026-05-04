#include "ImGuiSimpleEditor.h"
#include <misc/cpp/imgui_stdlib.h>

ImGuiSimpleEditor::ImGuiSimpleEditor() {}

static int countLines(const std::string& s) {
    if (s.empty()) return 1;
    int lines = 1;
    for (char c : s) if (c == '\n') ++lines;
    return lines;
}

void ImGuiSimpleEditor::Render(const char* id, std::string& text, const ImVec2& size) {
    ImVec2 avail = size;
    if (avail.x < 0 || avail.y < 0) avail = ImGui::GetContentRegionAvail();

    float gutter_width = 60.0f;
    ImGui::BeginChild(id, avail, false, ImGuiWindowFlags_NoMove);

    ImGui::BeginChild((std::string(id)+"_gutter").c_str(), ImVec2(gutter_width, 0), false);
    int lines = countLines(text);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.65f,0.65f,0.65f,1.0f));
    for (int i = 1; i <= lines; ++i) {
        ImGui::TextUnformatted(std::to_string(i).c_str());
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild((std::string(id)+"_code").c_str(), ImVec2(0,0), false);
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->Fonts.size() ? io.Fonts->Fonts[0] : ImGui::GetFont();
    ImGui::PushFont(font);
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    ImGui::InputTextMultiline((std::string(id)+"__input").c_str(), &text, ImVec2(-1,-1), flags);
    ImGui::PopFont();

    ImGui::EndChild();
    ImGui::EndChild();
}
