#include "imgui_utils.h"


void ImGui::Align(float width, ImGuiAlign alignment)
{
    ImGuiStyle &style = ImGui::GetStyle();

    float avail = ImGui::GetContentRegionAvail().x;
    float off = (avail - width) * (alignment * 0.5f);

    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

void ImGui::Align(const char *label, ImGuiAlign alignment)
{
    ImGuiStyle &style = ImGui::GetStyle();

    float width = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;
    float off = (avail - width) * (alignment * 0.5f);

    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

bool ImGui::CheckboxFilled(const char* label, bool* value)
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec2 label_size = ImGui::CalcTextSize(label);
    float box_sz = ImGui::GetFrameHeight();

    ImVec2 total_size = ImVec2(box_sz + style.ItemInnerSpacing.x + label_size.x, box_sz);
    ImVec2 p = ImGui::GetCursorScreenPos();

    ImGui::InvisibleButton(label, total_size);
    bool hovered = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked();
    if (clicked)
        *value = !*value;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImU32 border_col = ImGui::GetColorU32(ImGuiCol_Border);
    ImU32 fill_col   = ImGui::GetColorU32(*value ? ImGuiCol_CheckMark : ImGuiCol_FrameBg);
    ImU32 text_col   = ImGui::GetColorU32(ImGuiCol_Text);

    ImVec2 box_pos = p;
    ImVec2 box_max = ImVec2(p.x + box_sz, p.y + box_sz);

    draw_list->AddRectFilled(box_pos, box_max, ImGui::GetColorU32(ImGuiCol_FrameBg), style.FrameRounding);
    if (*value)
        draw_list->AddRectFilled(box_pos, box_max, fill_col, style.FrameRounding);
    draw_list->AddRect(box_pos, box_max, border_col, style.FrameRounding, 0, style.FrameBorderSize);

    ImVec2 text_pos = ImVec2(p.x + box_sz + style.ItemInnerSpacing.x, p.y + style.FramePadding.y);
    draw_list->AddText(text_pos, text_col, label);

    return *value;
}
