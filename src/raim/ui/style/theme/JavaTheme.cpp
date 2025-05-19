#include "JavaTheme.h"

#include <imgui.h>


JavaTheme::JavaTheme()
    : IRaimUITheme("Java")
{
}

void JavaTheme::apply() const
{
    IRaimUITheme::apply();

    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    colors[ImGuiCol_Text]                  = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.93f, 0.93f, 0.93f, 1.0f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.93f, 0.93f, 0.93f, 1.0f);
    colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    colors[ImGuiCol_TableRowBg]            = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_Border]                = ImVec4(0.70f, 0.70f, 0.70f, 1.0f);
    colors[ImGuiCol_CheckMark]             = ImVec4(0.1f, 0.5f, 1.0f, 1.0f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.26f, 0.50f, 0.90f, 1.0f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.30f, 0.60f, 1.00f, 1.0f);
    colors[ImGuiCol_Separator]             = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    colors[ImGuiCol_TitleBg]               = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);

    colors[ImGuiCol_FrameBg]               = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);

    colors[ImGuiCol_Button]                = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);

    colors[ImGuiCol_Tab]                   = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.70f, 0.70f, 0.70f, 1.0f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);

    colors[ImGuiCol_Header]                = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.70f, 0.70f, 0.70f, 1.0f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);
}
