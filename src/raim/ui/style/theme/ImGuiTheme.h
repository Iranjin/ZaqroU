#pragma once

#include <imgui.h>

#include "../IRaimUI_Theme.h"


class ImGuiTheme : public IRaimUITheme
{
public:
    ImGuiTheme()
        : IRaimUITheme("ImGui")
    {
    }

    void Apply() const override
    {
        ImGui::GetStyle() = ImGuiStyle();
        ImGui::StyleColorsDark();
    }
};
