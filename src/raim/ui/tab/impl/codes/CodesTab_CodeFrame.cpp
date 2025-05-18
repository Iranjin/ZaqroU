#include "CodesTab.h"

#include <filesystem>

#include <utils/tcp_gecko/TCPGecko.h>
#include <utils/Config.h>
#include "../../../RaimUI.h"
#include "../../../../Raim.h"

#include <imgui.h>


std::string CodesTab::CodeFrame_GetSelectedText(std::vector<std::string> &lines, int &selection_start, int &selection_end)
{
    if (selection_start == -1 || selection_end == -1)
        return "";

    int sel_start = std::min(selection_start, selection_end);
    int sel_end = std::max(selection_start, selection_end);

    std::string result;
    int current_chars = 0;
    for (size_t i = 0; i < lines.size(); ++i)
    {
        const std::string &line = lines[i];
        int line_length = line.length() + 1;

        if (sel_end < current_chars) break;
        if (sel_start > current_chars + line_length)
        {
            current_chars += line_length;
            continue;
        }

        int line_sel_start = std::max(0, sel_start - current_chars);
        int line_sel_end = std::min((int) line.length(), sel_end - current_chars);

        if (line_sel_start < line_sel_end)
        {
            result += line.substr(line_sel_start, line_sel_end - line_sel_start);
            if (i < lines.size() - 1 && sel_end > current_chars + line_sel_end)
                result += "\n";
        }

        current_chars += line_length;
    }

    return result;
};

int CodesTab::CodeFrame_GetCharIndexFromPos(std::vector<std::string> &lines, const ImVec2 &pos, ImVec2 &startPos, ImVec2 &textSize, ImVec2 &scroll_pos, float &lineHeight, float &lineNumberWidth)
{
    int line_idx = (pos.y - startPos.y + scroll_pos.y) / lineHeight;
    if (line_idx < 0) line_idx = 0;
    if (line_idx >= lines.size()) line_idx = lines.size() - 1;

    float x_offset = pos.x - (startPos.x + lineNumberWidth - scroll_pos.x);
    int char_idx = x_offset / textSize.x;
    if (char_idx < 0) char_idx = 0;
    if (char_idx >= lines[line_idx].length())
        char_idx = lines[line_idx].length();

    int total_chars = 0;
    for (int i = 0; i < line_idx; ++i)
        total_chars += lines[i].length() + 1;
    total_chars += char_idx;

    return total_chars;
}

void CodesTab::CodeFrame_DrawTextSelection(
    ImDrawList *draw_list,
    const std::vector<std::string> &lines,
    const ImVec2 &start_pos,
    float line_number_width,
    float line_height,
    const ImVec2 &char_size,
    const ImVec2 &scroll_pos,
    int sel_start,
    int sel_end)
{
    int current_chars = 0;
    for (size_t i = 0; i < lines.size(); ++i)
    {
        const std::string &line = lines[i];
        int line_length = line.length() + 1; // +1 for newline

        if (sel_end < current_chars) break;
        if (sel_start > current_chars + line_length)
        {
            current_chars += line_length;
            continue;
        }

        int line_sel_start = std::max(0, sel_start - current_chars);
        int line_sel_end = std::min(line_length, sel_end - current_chars);

        if (line_sel_start < line_sel_end)
        {
            ImVec2 rect_min = ImVec2(
                start_pos.x + line_number_width + line_sel_start * char_size.x - scroll_pos.x,
                start_pos.y + i * line_height - scroll_pos.y
            );
            ImVec2 rect_max = ImVec2(
                start_pos.x + line_number_width + line_sel_end * char_size.x - scroll_pos.x,
                rect_min.y + line_height
            );

            draw_list->AddRectFilled(
                rect_min,
                rect_max,
                ImGui::GetColorU32(ImGuiCol_TextSelectedBg)
            );
        }
        current_chars += line_length;
    }
}

void CodesTab::CodeFrame_ContextMenu(std::vector<std::string> &lines, int &selection_start, int &selection_end)
{
    if (ImGui::BeginPopupContextItem("code_context_menu"))
    {
        std::string selected_text = CodeFrame_GetSelectedText(lines, selection_start, selection_end);
        bool has_selection = !selected_text.empty();

        if (ImGui::MenuItem("Copy", NULL, false, has_selection))
            ImGui::SetClipboardText(selected_text.c_str());

        ImGui::EndPopup();
    }
}

void CodesTab::CodeFrame()
{
    if (mActiveIndex == -1)
        return;

    const std::string& code = mCodes[mActiveIndex].codes;
    static int selection_start = -1;
    static int selection_end = -1;
    static bool dragging = false;

    if (code.empty())
        return;

    std::vector<std::string> lines;
    std::stringstream ss(code);
    std::string line;
    
    while (std::getline(ss, line))
        lines.push_back(line);

    float lineNumberWidth = 50.0f;
    float lineHeight = ImGui::GetTextLineHeight();
    ImVec2 textSize = ImGui::CalcTextSize(" ");
    ImVec2 startPos = ImGui::GetCursorScreenPos();
    ImVec2 windowSize = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("Scrolling", windowSize, ImGuiChildFlags_None);
    ImVec2 scroll_pos(0.0f, ImGui::GetScrollY());

    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    ImGui::InvisibleButton("CodeArea", ImVec2(windowSize.x + scroll_pos.x, windowSize.y + scroll_pos.y));

    // 選択
    if (ImGui::IsItemFocused())
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            ImVec2 mouse_pos = ImGui::GetMousePos();
            selection_start = selection_end = CodeFrame_GetCharIndexFromPos(
                lines, mouse_pos, startPos, textSize, scroll_pos, lineHeight, lineNumberWidth);
            dragging = true;
        }
        else if (dragging && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        {
            ImVec2 mouse_pos = ImGui::GetMousePos();
            selection_end = CodeFrame_GetCharIndexFromPos(
                lines, mouse_pos, startPos, textSize, scroll_pos, lineHeight, lineNumberWidth);

            ImVec2 mouse_rel = ImVec2(mouse_pos.x - startPos.x, mouse_pos.y - startPos.y);
            constexpr float scroll_speed = 10.0f;

            if (mouse_rel.y < 0)
                ImGui::SetScrollY(ImGui::GetScrollY() + mouse_rel.y - scroll_speed);
            else if (mouse_rel.y > windowSize.y)
                ImGui::SetScrollY(ImGui::GetScrollY() + mouse_rel.y + scroll_speed - windowSize.y);
        }
        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            dragging = false;
        }
    }

    CodeFrame_ContextMenu(lines, selection_start, selection_end);

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) && ImGui::GetIO().KeyCtrl) 
    {
        if (ImGui::IsKeyPressed(ImGuiKey_A))
        {
            selection_start = 0;
            int total_chars = 0;
            for (const std::string &l : lines)
                total_chars += l.length() + 1;
            selection_end = total_chars;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_C))
        {
            std::string selected_text = CodeFrame_GetSelectedText(lines, selection_start, selection_end);
            if (!selected_text.empty())
                ImGui::SetClipboardText(selected_text.c_str());
        }
    }

    if (selection_start != -1 && selection_end != -1)
    {
        int sel_start = std::min(selection_start, selection_end);
        int sel_end = std::max(selection_start, selection_end);
        CodeFrame_DrawTextSelection(draw_list,
                                    lines,
                                    startPos,
                                    lineNumberWidth,
                                    lineHeight,
                                    textSize,
                                    scroll_pos,
                                    sel_start,
                                    sel_end);
    }

    for (size_t i = 0; i < lines.size(); ++i)
    {
        ImGui::SetCursorScreenPos(ImVec2(startPos.x - scroll_pos.x,
                                         startPos.y + i * lineHeight - scroll_pos.y));
        ImGui::Text("%4zu", i + 1);

        ImGui::SameLine(lineNumberWidth);
        ImGui::SetCursorScreenPos(ImVec2(startPos.x + lineNumberWidth - scroll_pos.x,
                                         startPos.y + i * lineHeight - scroll_pos.y));
        ImGui::TextUnformatted(lines[i].c_str());
    }

    ImGui::EndChild();
}

