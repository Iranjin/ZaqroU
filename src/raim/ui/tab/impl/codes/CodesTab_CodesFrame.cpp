#include "CodesTab.h"

#include <filesystem>
#include <algorithm>
#include <cctype>
#include <format>

#include <utils/tcp_gecko/TCPGecko.h>
#include <utils/Config.h>
#include <utils/StrUtils.h>
#include "../../../RaimUI.h"
#include "../../../../Raim.h"

#include <imgui_internal.h>
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

            SaveCodes(true);
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
        ImGui::OpenPopup("codes_frame_context_menu");
    }
}

void CodesTab::CodesFrame_DragAndDrop(size_t &i)
{
    if (ImGui::BeginDragDropSource())
    {
        std::vector<size_t> drag_indices;
        for (size_t idx : m_selected_indices)
        {
            if (idx >= 0 && idx < m_codes.size())
                drag_indices.push_back(idx);
        }

        if (!drag_indices.empty())
        {
            std::sort(drag_indices.begin(), drag_indices.end());

            ImGui::SetDragDropPayload("ENTRIES_MOVE", drag_indices.data(), drag_indices.size() * sizeof(int));
            
            if (drag_indices.size() == 1)
                ImGui::Text("Move Entry: %s", m_codes[drag_indices[0]].name.c_str());
            else
                ImGui::Text("Move %zu Entries", drag_indices.size());
        }

        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTRIES_MOVE"))
        {
            const int *src_indices = static_cast<const int*>(payload->Data);
            size_t count = payload->DataSize / sizeof(int);
            std::vector<size_t> indices;

            for (size_t k = 0; k < count; ++k)
            {
                size_t idx = src_indices[k];
                if (idx >= 0 && idx < m_codes.size())
                    indices.push_back(idx);
            }

            if (indices.empty())
                return;

            std::sort(indices.begin(), indices.end());

            std::vector<CodeEntry> moving_entries;
            for (size_t idx : indices)
                moving_entries.push_back(m_codes[idx]);

            m_codes.begin_modify();
            for (auto it = indices.rbegin(); it != indices.rend(); ++it)
                m_codes.remove_entry(*it);

            size_t insert_pos = std::min(i, m_codes.size());

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
    if (ImGui::BeginPopup("codes_frame_context_menu"))
    {
        if (m_show_search_bar && m_filtered_indices.size() < m_codes.size())
        {
            if (ImGui::MenuItem("↪ Go to Selected"))
            {
                m_filtered_indices.clear();
                m_show_search_bar = false;
                m_search_query[0] = '\0';

                CodesFrame_ScrollToIndex(m_active_index);
            }
            ImGui::Separator();
        }
        if (ImGui::MenuItem("Edit"))
        {
            if (!m_selected_indices.empty())
            {
                size_t selected = *m_selected_indices.begin();
                OpenEditCodeWindow(selected);
            }
        }
        if (ImGui::MenuItem("Copy"))
        {
            std::vector<std::string> result;

            size_t count = 0;
            for (const size_t &selected_index : m_selected_indices)
            {
                const CodeEntry &entry = m_codes[selected_index];
                if (!entry.empty())
                {
                    std::string first_line;
                    if (!entry.name.empty())
                        first_line += entry.name;

                    if (!entry.authors.empty())
                    {
                        std::vector<std::string> parts = split(entry.authors, ",");

                        std::vector<std::string> trimmed_authors;
                        for (const std::string &name : parts)
                            trimmed_authors.push_back(trim(name));

                        if (!trimmed_authors.empty())
                        {
                            if (!first_line.empty())
                                first_line += ' ';
                            first_line += '[' + join(trimmed_authors, ", ") + ']';
                        }
                    }

                    if (!first_line.empty())
                        result.push_back(first_line);
                    if (!entry.codes.empty())
                        result.push_back(entry.codes);
                    if (!entry.comment.empty())
                        result.push_back(entry.comment);

                    if (count != m_selected_indices.size() - 1)
                        result.emplace_back("");
                }
                
                ++count;
            }

            ImGui::SetClipboardText(join(result, "\n").c_str());
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
        ImGui::EndPopup();
    }
}

void CodesTab::CodesFrame()
{
    ImGui::BeginChild("CodesFrameChild", ImVec2(0, 0));
    
    ImGuiIO &io = ImGui::GetIO();

    // ドラッグ中の自動スクロール
    if (ImGui::IsDragDropActive())
    {
        const ImVec2 mouse_pos = ImGui::GetMousePos();
        const ImVec2 window_pos = ImGui::GetWindowPos();
        const ImVec2 window_size = ImGui::GetWindowSize();

        constexpr float base_scroll_speed = 300.0f;
        constexpr float scroll_threshold = 20.0f;

        float scroll_y = ImGui::GetScrollY();
        float delta_time = io.DeltaTime;

        if (mouse_pos.y < window_pos.y + scroll_threshold) // 上
        {
            float distance = (window_pos.y + scroll_threshold) - mouse_pos.y;
            float speed = base_scroll_speed * (distance / scroll_threshold);
            scroll_y -= speed * delta_time;
            if (scroll_y < 0)
                scroll_y = 0;
            ImGui::SetScrollY(scroll_y);
        }
        else if (mouse_pos.y > window_pos.y + window_size.y - scroll_threshold) // 下
        {
            float distance = mouse_pos.y - (window_pos.y + window_size.y - scroll_threshold);
            float speed = base_scroll_speed * (distance / scroll_threshold);
            float max_scroll = ImGui::GetScrollMaxY();
            scroll_y += speed * delta_time;
            if (scroll_y > max_scroll)
                scroll_y = max_scroll;
            ImGui::SetScrollY(scroll_y);
        }
    }

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
                bool new_state = !m_codes[*m_selected_indices.begin()].enabled;

                m_codes.begin_modify();
                for (size_t i : m_selected_indices)
                    m_codes[i].enabled = new_state;
                m_codes.end_modify();

                SaveCodes(true);
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
                size_t current_filtered_pos = -1;
                for (size_t j = 0; j < m_filtered_indices.size(); ++j)
                {
                    if (m_filtered_indices[j] == m_active_index)
                    {
                        current_filtered_pos = j;
                        break;
                    }
                }

                size_t new_filtered_pos = -1;
                if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
                {
                    if (current_filtered_pos > 0)
                        new_filtered_pos = current_filtered_pos - 1;
                }
                else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
                {
                    if (current_filtered_pos >= 0 && current_filtered_pos < m_filtered_indices.size() - 1)
                        new_filtered_pos = current_filtered_pos + 1;
                }

                if (new_filtered_pos != -1)
                {
                    size_t new_index = m_filtered_indices[new_filtered_pos];
                    m_selected_indices.clear();
                    m_selected_indices.insert(new_index);
                    m_active_index = new_index;

                    float item_height = ImGui::GetFrameHeightWithSpacing();
                    float scrollY = ImGui::GetScrollY();
                    float maxScrollY = ImGui::GetScrollMaxY();
                    float windowHeight = ImGui::GetWindowHeight();

                    float itemYPos = item_height * new_filtered_pos;

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

            if (ImGui::IsKeyPressed(ImGuiKey_Z))
            {
                if (io.KeyShift)
                    m_codes.redo();
                else
                    m_codes.undo();
                
                SaveCodes(true);

                m_selected_indices.clear();
            }
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

    ImVec2 full_width = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeightWithSpacing());

    for (size_t i = 0; i < m_filtered_indices.size(); ++i)
    {
        size_t index = m_filtered_indices[i];
        
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

        if (full_width.x > 0)  // 0以下でボタンを配置しようとするとクラッシュすることがある
            ImGui::InvisibleButton("##EntryWidget", ImVec2(full_width.x, ImGui::GetFrameHeight()));

        CodesFrame_MouseClick(io, is_selected, index);
        CodesFrame_DragAndDrop(index);

        std::string label = m_codes[index].name;

        if (!m_codes[index].authors.empty())
        {
            std::vector<std::string> parts = split(m_codes[index].authors, ",");
            std::vector<std::string> trimmed_authors;

            for (std::string &name : parts)
                trimmed_authors.push_back(trim(name));

            if (!trimmed_authors.empty())
                label += " [" + join(trimmed_authors, ", ") + "]";
        }

        float max_label_width = full_width.x - ImGui::GetStyle().FramePadding.x * 2.0f - ImGui::GetFontSize();
        ImVec2 textSize = ImGui::CalcTextSize(label.c_str());

        std::string display_label = label;
        bool was_truncated = false;
        if (textSize.x > max_label_width)
        {
            was_truncated = true;
            const char *str = label.c_str();
            const char *str_end = str + label.size();

            std::string temp;
            while (str < str_end)
            {
                const char *prev = str;
                unsigned int c;
                int char_len = ImTextCharFromUtf8(&c, str, str_end);
                if (char_len == 0)
                    break;

                str += char_len;

                std::string next = temp + std::string(prev, str) + "...";
                if (ImGui::CalcTextSize(next.c_str()).x > max_label_width)
                    break;

                temp += std::string(prev, str);
            }
            display_label = temp + "...";
        }

        ImGui::SetCursorPos(checkboxPos);
        ImGui::Checkbox(display_label.c_str(), &m_codes[index].enabled);

        if (was_truncated && ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", label.c_str());

        CodesFrame_ContextMenu();

        ImGui::PopID();
    }

    if (m_list_updated)
    {
        ImGui::SetScrollHereY(1.0f);
        m_list_updated = false;
    }

    ImGui::EndChild();
}
