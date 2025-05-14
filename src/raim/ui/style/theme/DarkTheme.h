#pragma once

#include <imgui.h>

#include "../IRaimUI_Theme.h"


class DarkTheme : public IRaimUITheme
{
public:
    DarkTheme()
        : IRaimUITheme("Dark")
    {
    }

    void Apply() const override
    {
        IRaimUITheme::Apply();

        ImGuiStyle &style = ImGui::GetStyle();
        ImVec4 *colors = style.Colors;

        colors[ImGuiCol_Text]                  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[ImGuiCol_WindowBg]              = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        colors[ImGuiCol_PopupBg]               = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        colors[ImGuiCol_TableRowBg]            = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        colors[ImGuiCol_Border]                = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
        colors[ImGuiCol_CheckMark]             = ImVec4(0.1f, 0.5f, 1.0f, 1.0f);
        colors[ImGuiCol_SliderGrab]            = ImVec4(0.26f, 0.50f, 0.90f, 1.0f);
        colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.30f, 0.60f, 1.00f, 1.0f);
        colors[ImGuiCol_Separator]             = ImVec4(0.40f, 0.40f, 0.40f, 1.0f);
        colors[ImGuiCol_DragDropTarget]        = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    
        colors[ImGuiCol_TitleBg]               = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
        colors[ImGuiCol_TitleBgActive]         = ImVec4(0.50f, 0.50f, 0.50f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);

        colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
        colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
        colors[ImGuiCol_FrameBgActive]         = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);

        colors[ImGuiCol_Button]                = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        colors[ImGuiCol_ButtonHovered]         = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        colors[ImGuiCol_ButtonActive]          = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

        colors[ImGuiCol_Tab]                   = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        colors[ImGuiCol_TabHovered]            = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        colors[ImGuiCol_TabActive]             = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

        colors[ImGuiCol_Header]                = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        colors[ImGuiCol_HeaderHovered]         = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        colors[ImGuiCol_HeaderActive]          = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    }
};
