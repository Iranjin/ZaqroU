#include "ImGuiTheme.h"

#include <imgui.h>


ImGuiTheme::ImGuiTheme()
    : IRaimUITheme("ImGui")
{
}

void ImGuiTheme::Apply() const
{
    ImGui::GetStyle() = ImGuiStyle();
    ImGui::StyleColorsDark();
}
