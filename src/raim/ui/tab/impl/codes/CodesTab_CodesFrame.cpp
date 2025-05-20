#include "CodesTab.h"

#include <filesystem>
#include <algorithm>
#include <cctype>
#include <format>

#include <utils/tcp_gecko/TCPGecko.h>
#include <utils/Config.h>
#include "../../../RaimUI.h"
#include "../../../../Raim.h"

#include <imgui.h>


void CodesTab::CodesFrame_Search()
{
    ImGuiIO &io = ImGui::GetIO();

    static bool search_bar_first_time = false;
    
    if (m_show_search_bar)
    {
        if (search_bar_first_time)
        {
            ImGui::SetKeyboardFocusHere();
            search_bar_first_time = false;
        }

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputTextWithHint("##SearchBar", "Search...", m_search_query, IM_ARRAYSIZE(m_search_query));

        if (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Escape))
            m_show_search_bar = false;

        m_filtered_indices.clear();
        for (size_t i = 0; i < m_codes.size(); ++i)
        {
            std::string name, authors_combined, query;

            for (char c : m_codes[i].name)
                if (!std::isspace(c))
                    name += std::tolower(c);

            for (char c : m_codes[i].authors)
                if (!std::isspace(c))
                    authors_combined += std::tolower(c);

            std::string query_raw(m_search_query);
            for (char c : query_raw)
                if (!std::isspace(c))
                    query += std::tolower(c);

            if (query.empty() ||
                name.find(query) != std::string::npos ||
                authors_combined.find(query) != std::string::npos)
            {
                m_filtered_indices.push_back(i);
            }
        }

    }
    else
    {
        m_filtered_indices.clear();
        for (size_t i = 0; i < m_codes.size(); ++i)
            m_filtered_indices.push_back(i);
        
        search_bar_first_time = true;
    }
}

void CodesTab::CodesFrame_MouseClick(ImGuiIO &io, bool &is_selected, size_t &i)
{
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        if (io.KeyCtrl) // 複数選択
        {
            if (is_selected)
                m_selected_indices.erase(i);
            else
                m_selected_indices.insert(i);
        }
        else if (io.KeyShift) // 範囲選択
        {
            if (m_selected_indices.empty()) 
            {
                m_selected_indices.insert(i);
            }
            else
            {
                size_t start = std::min(m_active_index, i);
                size_t end = std::max(m_active_index, i);
                
                for (size_t j = start; j <= end; ++j)
                    m_selected_indices.insert(j);
            }
        }
        else // 単一選択
        {
            if (m_selected_indices.count(i) == 0)
            {
                m_selected_indices.clear();
                m_selected_indices.insert(i);
            }
            m_codes.begin_modify();
            m_codes[i].enabled = !m_codes[i].enabled;
            m_codes.end_modify();
        }

        m_active_index = i;
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        m_active_index = i;
        if (m_selected_indices.size() <= 1)
        {
            m_selected_indices.clear();
            m_selected_indices.insert(i);
        }
        ImGui::OpenPopup("RightClickMenu");
    }
}

void CodesTab::CodesFrame_DragAndDrop(size_t &i)
{
    if (ImGui::BeginDragDropSource())
    {
        std::vector<int> drag_indices(m_selected_indices.begin(), m_selected_indices.end());
        std::sort(drag_indices.begin(), drag_indices.end());

        ImGui::SetDragDropPayload("ENTRIES_MOVE", drag_indices.data(), drag_indices.size() * sizeof(int));
        
        if (drag_indices.size() == 1)
            ImGui::Text("Move Entry: %s", m_codes[drag_indices[0]].name.c_str());
        else
            ImGui::Text("Move %zu Entries", drag_indices.size());

        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTRIES_MOVE"))
        {
            const int *src_indices = static_cast<const int*>(payload->Data);
            size_t count = payload->DataSize / sizeof(int);
            std::vector<int> indices(src_indices, src_indices + count);

            std::sort(indices.begin(), indices.end());

            std::vector<CodeEntry> moving_entries;
            for (int idx : indices)
                moving_entries.push_back(m_codes[idx]);

            m_codes.begin_modify();
            for (auto it = indices.rbegin(); it != indices.rend(); ++it)
                m_codes.remove_entry(*it);

            size_t insert_pos = i;
            for (size_t j = 0; j < moving_entries.size(); ++j)
                m_codes.insert_entry(insert_pos + j, moving_entries[j]);
            m_codes.end_modify();

            m_selected_indices.clear();
            for (size_t j = 0; j < moving_entries.size(); ++j)
                m_selected_indices.insert(insert_pos + j);
        }
        ImGui::EndDragDropTarget();
    }
}

void CodesTab::CodesFrame_ContextMenu()
{
    if (ImGui::BeginPopup("RightClickMenu"))
    {
        if (ImGui::MenuItem("Edit"))
        {
            if (!m_selected_indices.empty())
            {
                size_t selected = *m_selected_indices.begin();
                m_popup_entry = m_codes[selected];
                m_edit_target_index = selected;
                m_popup_mode = CodePopupMode::Edit;
                ImGui::OpenPopup("CodePopup");
            }
        }
        if (ImGui::MenuItem("Delete"))
        {
            std::vector<size_t> sorted_indices(m_selected_indices.begin(), m_selected_indices.end());
            std::sort(sorted_indices.begin(), sorted_indices.end());

            m_codes.begin_modify();
            for (auto it = sorted_indices.rbegin(); it != sorted_indices.rend(); ++it)
            {
                size_t selected_index = *it;
                m_codes.remove_entry(selected_index);
                m_selected_indices.erase(selected_index);
            }
            m_codes.end_modify();

            m_active_index = -1;
        }
        if (ImGui::MenuItem("Duplicate"))
        {
            std::vector<size_t> new_indices;

            m_codes.begin_modify();
            size_t current_index = m_codes.size();
            for (size_t selected_index : m_selected_indices)
            {
                m_codes.insert_entry(current_index, m_codes[selected_index]);
                new_indices.push_back(current_index);
                current_index++;
            }
            m_codes.end_modify();

            m_selected_indices.clear();
            for (size_t new_index : new_indices)
                m_selected_indices.insert(new_index);
        }

        ImGui::EndPopup();
    }
}

void CodesTab::CodesFrame()
{
    ImGui::BeginChild("CodesFrameChild", ImVec2(0, 0), ImGuiChildFlags_None);
    
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
    ImGuiIO &io = ImGui::GetIO();

    // ショートカット
    if (ImGui::IsWindowFocused() && !io.WantCaptureKeyboard)
    {
        if (!m_filtered_indices.empty())
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                m_selected_indices.clear();
                m_active_index = -1;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))
            {
                bool newState = !m_codes[*m_selected_indices.begin()].enabled;

                m_codes.begin_modify();
                for (size_t i : m_selected_indices)
                    m_codes[i].enabled = newState;
                m_codes.end_modify();
            }

            if (io.KeyCtrl)
            {
                if (ImGui::IsKeyPressed(ImGuiKey_A))
                {
                    m_selected_indices.clear();
                    for (size_t i : m_filtered_indices)
                        m_selected_indices.insert(i);
                    m_active_index = m_filtered_indices.empty() ? -1 : m_filtered_indices.back();
                }
            }

            if (!m_selected_indices.empty())
            {
                size_t currentFilteredPos = -1;
                for (size_t j = 0; j < m_filtered_indices.size(); ++j)
                {
                    if (m_filtered_indices[j] == m_active_index)
                    {
                        currentFilteredPos = j;
                        break;
                    }
                }

                size_t newFilteredPos = -1;
                if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
                {
                    if (currentFilteredPos > 0)
                        newFilteredPos = currentFilteredPos - 1;
                }
                else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
                {
                    if (currentFilteredPos >= 0 && currentFilteredPos < m_filtered_indices.size() - 1)
                        newFilteredPos = currentFilteredPos + 1;
                }

                if (newFilteredPos != -1)
                {
                    size_t new_index = m_filtered_indices[newFilteredPos];
                    m_selected_indices.clear();
                    m_selected_indices.insert(new_index);
                    m_active_index = new_index;

                    float item_height = ImGui::GetFrameHeightWithSpacing();
                    float scrollY = ImGui::GetScrollY();
                    float maxScrollY = ImGui::GetScrollMaxY();
                    float windowHeight = ImGui::GetWindowHeight();

                    float itemYPos = item_height * newFilteredPos;

                    if (itemYPos < scrollY)
                        ImGui::SetScrollY(itemYPos);
                    else if (itemYPos + item_height > scrollY + windowHeight)
                        ImGui::SetScrollY(itemYPos + item_height - windowHeight);
                }
            }
        }

        if (io.KeyCtrl)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_F)) // Search
                m_show_search_bar = !m_show_search_bar;
            
            if (ImGui::IsKeyPressed(ImGuiKey_Z)) // Undo
                m_codes.undo();
            
            if (io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z)) // Redo
                m_codes.redo();
        }
    }

    if (m_scroll_request.requested)
    {
        auto it = std::find(m_filtered_indices.begin(), m_filtered_indices.end(), m_scroll_request.target_index);
        if (it != m_filtered_indices.end())
        {
            size_t filtered_pos = std::distance(m_filtered_indices.begin(), it);
            float item_height = ImGui::GetFrameHeightWithSpacing();

            if (m_scroll_request.align_bottom)
                ImGui::SetScrollY(item_height * (m_filtered_indices.size() - filtered_pos));
            else
                ImGui::SetScrollY(item_height * filtered_pos);
        }

        m_scroll_request.reset();
    }

    for (size_t i = 0; i < m_filtered_indices.size(); ++i)
    {
        size_t index = m_filtered_indices[i];

        ImVec2 full_width = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeightWithSpacing());
        bool is_selected = m_selected_indices.count(index) > 0;

        if (is_selected)
        {
            ImU32 color = ImGui::GetColorU32(ImGuiCol_Header);
            ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(
                cursor_pos,
                ImVec2(cursor_pos.x + full_width.x, cursor_pos.y + ImGui::GetFrameHeight()),
                ImColor(ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]),
                ImGui::GetStyle().FrameRounding
            );
        }

        ImGui::PushID(i);

        ImVec2 checkboxPos = ImGui::GetCursorPos();
        ImGui::InvisibleButton("##EntryWidget", ImVec2(full_width.x, ImGui::GetFrameHeight()));

        CodesFrame_MouseClick(io, is_selected, index);
        CodesFrame_DragAndDrop(index);

        std::string label = m_codes[index].name;
        if (!m_codes[index].authors.empty())
            label += std::format(" [{}]", m_codes[index].authors);

        std::string display_label = label;

        float max_label_width = full_width.x - ImGui::GetStyle().FramePadding.x * 2.0f - ImGui::GetFontSize();
        ImVec2 textSize = ImGui::CalcTextSize(label.c_str());

        bool was_truncated = false;
        if (textSize.x > max_label_width)
        {
            was_truncated = true;
            while (display_label.length() > 3 &&
                   ImGui::CalcTextSize((display_label.substr(0, display_label.length() - 3) + "...").c_str()).x > max_label_width)
                display_label.pop_back();
            display_label = display_label.substr(0, display_label.length() - 3) + "...";
        }

        ImGui::SetCursorPos(checkboxPos);
        ImGui::Checkbox(display_label.c_str(), &m_codes[index].enabled);

        if (was_truncated && ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", label.c_str());

        CodesFrame_ContextMenu();

        ImGui::PopID();
    }

    ImGui::PopStyleVar();

    if (m_list_updated)
    {
        ImGui::SetScrollHereY(1.0f);
        m_list_updated = false;
    }

    ImGui::EndChild();
}

