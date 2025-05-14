#pragma once

#include <string>
#include <unordered_set>

#include "backend/CodeLoader.h"
#include "../../IRaimTab.h"


class ImGuiIO;
class ImDrawList;
class ImVec2;

class TCPGecko;

class CodesTab : public IRaimTab
{
private:
    enum class CodePopupMode
    {
        None,
        Add,
        Edit
    };

    CodeEntryManager mCodes;

    CodePopupMode mPopupMode = CodePopupMode::None;
    CodeEntry mPopupEntry;
    size_t mEditTargetIndex = -1;

    std::unordered_set<size_t> mSelectedIndices;
    size_t mActiveIndex = -1;

    const char *mNotifTitle = "CodesTab";

    bool mShowSearchBar = false;
    char mSearchQuery[256] = "";
    std::vector<size_t> mFilteredIndices;

    bool mConnected = false,
         mLastConnected = false;
    std::string mLoadedPath;

    bool mListUpdated = false;
    
public:
    CodesTab(RaimUI *raimUI);

    std::shared_ptr<TCPGecko> getTCPGecko();

    void CodesFrame_MouseClick(ImGuiIO &io, bool &isSelected, size_t &i);
    void CodesFrame_DragAndDrop(size_t &i);
    void CodesFrame_ContextMenu();
    void CodesFrame_Search();
    void CodesFrame();

    std::string CodeFrame_GetSelectedText(std::vector<std::string> &lines, int &selection_start, int &selection_end);
    int CodeFrame_GetCharIndexFromPos(std::vector<std::string> &lines, const ImVec2 &pos, ImVec2 &startPos, ImVec2 &textSize, ImVec2 &scroll_pos, float &lineHeight, float &lineNumberWidth);
    void CodeFrame_DrawTextSelection(ImDrawList *draw_list, const std::vector<std::string> &lines, const ImVec2 &start_pos, float line_number_width, float line_height, const ImVec2 &char_size, const ImVec2 &scroll_pos, int sel_start, int sel_end);
    void CodeFrame_ContextMenu(std::vector<std::string> &lines, int &selection_start, int &selection_end);
    void CodeFrame();

    void CommentFrame();


    void CodesTable(const ImVec2 &available);
    void ControlsFrame(const ImVec2 &available);

    void CodePopup();

    void SendCodes();
    void DisableCodes();

    void Update() override;
    void OnConnected() override;
};
