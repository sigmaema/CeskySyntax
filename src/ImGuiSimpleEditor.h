// Lightweight ImGui editor widget with gutter (line numbers) and monospace font.
#pragma once
#include <string>
#include <imgui.h>

class ImGuiSimpleEditor {
public:
    ImGuiSimpleEditor();
    // Render editor into current ImGui window. `text` is edited in-place.
    void Render(const char* id, std::string& text, const ImVec2& size = ImVec2(-1, -1));
};
