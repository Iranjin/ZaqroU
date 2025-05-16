#include "GrayScaleTheme.h"

#include <imgui.h>


GrayScaleTheme::GrayScaleTheme()
    : IRaimUITheme("GrayScale")
{
}

void GrayScaleTheme::Apply() const
{
    IRaimUITheme::Apply();

    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    style.FrameBorderSize = 0.1f;
    style.ChildRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.WindowRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.FramePadding = ImVec2(10, 6);

    colors[ImGuiCol_Text]              = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_WindowBg]          = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    colors[ImGuiCol_PopupBg]           = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    colors[ImGuiCol_TableRowBg]        = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    colors[ImGuiCol_Border]            = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    colors[ImGuiCol_TitleBg]           = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    colors[ImGuiCol_TitleBgActive]     = ImVec4(0.50f, 0.50f, 0.50f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed]  = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);

    colors[ImGuiCol_FrameBg]           = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    colors[ImGuiCol_FrameBgHovered]    = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    colors[ImGuiCol_FrameBgActive]     = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);

    colors[ImGuiCol_Button]            = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    colors[ImGuiCol_ButtonHovered]     = ImVec4(0.50f, 0.50f, 0.50f, 1.0f);
    colors[ImGuiCol_ButtonActive]      = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);

    colors[ImGuiCol_SliderGrab]        = ImVec4(0.90f, 0.90f, 0.90f, 1.0f);
    colors[ImGuiCol_SliderGrabActive]  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    colors[ImGuiCol_Tab]               = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    colors[ImGuiCol_TabHovered]        = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    colors[ImGuiCol_TabActive]         = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);

    colors[ImGuiCol_Header]            = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
    colors[ImGuiCol_HeaderHovered]     = ImVec4(0.50f, 0.50f, 0.50f, 1.0f);
    colors[ImGuiCol_HeaderActive]      = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);

    colors[ImGuiCol_CheckMark]         = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_Separator]         = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
    colors[ImGuiCol_DragDropTarget]    = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}
