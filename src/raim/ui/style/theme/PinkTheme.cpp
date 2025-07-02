#include "PinkTheme.h"

#include <imgui.h>


PinkTheme::PinkTheme()
    : IRaimUITheme("Pink")
{
}

void PinkTheme::apply() const
{
    IRaimUITheme::apply();

    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    colors[ImGuiCol_Text]                  = ImVec4(1.0f, 0.9f, 0.95f, 1.0f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    colors[ImGuiCol_Border]                = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);

    colors[ImGuiCol_CheckMark]             = ImVec4(1.0f, 0.3f, 0.6f, 1.0f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(1.0f, 0.4f, 0.7f, 1.0f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(1.0f, 0.2f, 0.5f, 1.0f);
    colors[ImGuiCol_Separator]             = ImVec4(0.6f, 0.3f, 0.5f, 1.0f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(1.0f, 0.5f, 0.7f, 1.0f);

    colors[ImGuiCol_TitleBg]               = ImVec4(0.6f, 0.2f, 0.4f, 1.0f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.8f, 0.3f, 0.5f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.4f, 0.15f, 0.3f, 1.0f);

    colors[ImGuiCol_FrameBg]               = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);

    colors[ImGuiCol_Button]                = ImVec4(0.8f, 0.3f, 0.6f, 1.0f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(1.0f, 0.4f, 0.7f, 1.0f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.9f, 0.2f, 0.6f, 1.0f);

    colors[ImGuiCol_Tab]                   = ImVec4(0.6f, 0.2f, 0.4f, 1.0f);
    colors[ImGuiCol_TabHovered]            = ImVec4(1.0f, 0.3f, 0.6f, 1.0f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.8f, 0.2f, 0.5f, 1.0f);

    colors[ImGuiCol_Header]                = ImVec4(0.7f, 0.3f, 0.5f, 1.0f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(1.0f, 0.4f, 0.7f, 1.0f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.9f, 0.2f, 0.6f, 1.0f);
}
