#include "IRaimUI_Theme.h"

#include <imgui.h>


void IRaimUITheme::Apply() const // Default
{
    ImGuiStyle &style = ImGui::GetStyle();

    style.FrameBorderSize = 0.025f;
    style.ChildRounding = 4.0f;
    style.TabRounding = 4.0f;
    style.WindowRounding = 8.0f;
    style.GrabRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.FramePadding = ImVec2(10, 6);
    
    ImVec4 *colors = style.Colors;

    colors[ImGuiCol_Text]                  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_WindowBg]              = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    colors[ImGuiCol_PopupBg]               = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    colors[ImGuiCol_TableRowBg]            = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    colors[ImGuiCol_Border]                = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    colors[ImGuiCol_CheckMark]             = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    colors[ImGuiCol_SliderGrab]            = ImVec4(0.7f,  0.2f,  0.2f,  1.0f);
    colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.8f,  0.3f,  0.3f,  1.0f);
    colors[ImGuiCol_Separator]             = ImVec4(0.40f, 0.40f, 0.40f, 1.0f);
    colors[ImGuiCol_DragDropTarget]        = ImVec4(0.7f,  0.2f,  0.2f,  1.0f);
    
    colors[ImGuiCol_TitleBg]               = ImVec4(0.7f,  0.2f,  0.2f,  1.0f);
    colors[ImGuiCol_TitleBgActive]         = ImVec4(0.4f,  0.1f,  0.1f,  1.0f);
    colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.8f,  0.3f,  0.3f,  1.0f);

    colors[ImGuiCol_FrameBg]               = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
    colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    colors[ImGuiCol_FrameBgActive]         = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);

    colors[ImGuiCol_Button]                = ImVec4(0.7f,  0.2f,  0.2f,  1.0f);
    colors[ImGuiCol_ButtonHovered]         = ImVec4(0.8f,  0.3f,  0.3f,  1.0f);
    colors[ImGuiCol_ButtonActive]          = ImVec4(0.4f,  0.1f,  0.1f,  1.0f);

    colors[ImGuiCol_Tab]                   = ImVec4(0.7f, 0.2f, 0.2f, 1.0f);
    colors[ImGuiCol_TabHovered]            = ImVec4(0.8f, 0.3f, 0.3f, 1.0f);
    colors[ImGuiCol_TabActive]             = ImVec4(0.4f, 0.1f, 0.1f, 1.0f);

    colors[ImGuiCol_Header]                = ImVec4(0.7f, 0.2f, 0.2f, 1.0f);
    colors[ImGuiCol_HeaderHovered]         = ImVec4(0.8f, 0.3f, 0.3f, 1.0f);
    colors[ImGuiCol_HeaderActive]          = ImVec4(0.4f, 0.1f, 0.1f, 1.0f);
}
