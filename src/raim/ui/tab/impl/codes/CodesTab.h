#pragma once

#include <string>
#include <unordered_set>

#include "backend/CodeEntryManager.h"
#include "../../IRaimTab.h"


class ImGuiIO;
class ImDrawList;
class ImVec2;

class TCPGecko;

class CodesTab : public IRaimTab
{
private:
    struct ScrollRequest
    {
        bool requested = false;
        size_t target_index = 0;
        bool align_bottom = false;

        void reset()
        {
            requested = false;
            target_index = 0;
            align_bottom = false;
        }
    };

    ScrollRequest m_scroll_request;

    enum class CodeWindowMode
    {
        None,
        Add,
        Edit
    };

    CodeEntryManager m_codes;

    CodeWindowMode m_code_window_mode = CodeWindowMode::None;
    CodeEntry m_code_window_entry;
    size_t m_edit_target_index = -1;

    std::unordered_set<size_t> m_selected_indices;
    size_t m_active_index = -1;

    const char *m_notif_title = "CodesTab";

    bool m_show_search_bar = false;
    char m_search_query[256] = "";
    std::vector<size_t> m_filtered_indices;

    bool m_connected = false,
         m_last_connected = false;
    std::string m_loaded_path;

    bool m_list_updated = false;
    
public:
    CodesTab(RaimUI *raim_ui);

    std::shared_ptr<TCPGecko> get_tcp_gecko();

    void CodesFrame_MouseClick(ImGuiIO &io, bool &is_selected, size_t &i);
    void CodesFrame_DragAndDrop(size_t &i);
    void CodesFrame_ContextMenu();
    void CodesFrame_Search();
    void CodesFrame();

    std::string CodeFrame_GetSelectedText(std::vector<std::string> &lines, int &selection_start, int &selection_end);
    int CodeFrame_GetCharIndexFromPos(std::vector<std::string> &lines, const ImVec2 &pos, ImVec2 &start_pos, ImVec2 &text_size, ImVec2 &scroll_pos, float &line_height, float &line_number_width);
    void CodeFrame_DrawTextSelection(ImDrawList *draw_list, const std::vector<std::string> &lines, const ImVec2 &start_pos, float line_number_width, float line_height, const ImVec2 &char_size, const ImVec2 &scroll_pos, int sel_start, int sel_end);
    void CodeFrame_ContextMenu(std::vector<std::string> &lines, int &selection_start, int &selection_end);
    void CodeFrame();

    void CommentFrame();

    void CodesTable(const ImVec2 &available);
    void ControlsFrame(const ImVec2 &available);
    void CodeWindow();
    
    void CodesFrame_ScrollToIndex(size_t index, bool align_bottom = false);

    void SaveCodes(bool check_auto_save = false);

    void SendCodes();
    void DisableCodes();

    void Update() override;
    void UpdateBackground() override;
    void OnConnected() override;
};
