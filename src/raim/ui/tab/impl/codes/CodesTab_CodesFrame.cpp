#include "CodesTab.h"

#include <filesystem>
#include <algorithm>
#include <cctype>

#include <utils/tcp_gecko/TCPGecko.h>
#include <utils/Config.h>
#include "../../../RaimUI.h"
#include "../../../../Raim.h"

#include <imgui.h>


void CodesTab::CodesFrame_Search()
{
    ImGuiIO &io = ImGui::GetIO();

    static bool searchBarFirstTime = false;
    
    if (mShowSearchBar)
    {
        if (searchBarFirstTime)
        {
            ImGui::SetKeyboardFocusHere();
            searchBarFirstTime = false;
        }

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputTextWithHint("##SearchBar", "Search...", mSearchQuery, IM_ARRAYSIZE(mSearchQuery));

        if (ImGui::IsItemFocused() && ImGui::IsKeyPressed(ImGuiKey_Escape))
            mShowSearchBar = false;

        mFilteredIndices.clear();
        for (size_t i = 0; i < mCodes.size(); ++i)
        {
            std::string name, authorsCombined, query;

            for (char c : mCodes[i].name)
                if (!std::isspace(c))
                    name += std::tolower(c);

            for (char c : mCodes[i].authors)
                if (!std::isspace(c))
                    authorsCombined += std::tolower(c);

            std::string query_raw(mSearchQuery);
            for (char c : query_raw)
                if (!std::isspace(c))
                    query += std::tolower(c);

            if (query.empty() ||
                name.find(query) != std::string::npos ||
                authorsCombined.find(query) != std::string::npos)
            {
                mFilteredIndices.push_back(i);
            }
        }

    }
    else
    {
        mFilteredIndices.clear();
        for (size_t i = 0; i < mCodes.size(); ++i)
            mFilteredIndices.push_back(i);
        
        searchBarFirstTime = true;
    }
}

void CodesTab::CodesFrame_MouseClick(ImGuiIO &io, bool &isSelected, size_t &i)
{
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        if (io.KeyCtrl) // 複数選択
        {
            if (isSelected)
                mSelectedIndices.erase(i);
            else
                mSelectedIndices.insert(i);
        }
        else if (io.KeyShift) // 範囲選択
        {
            if (mSelectedIndices.empty()) 
            {
                mSelectedIndices.insert(i);
            }
            else
            {
                size_t start = std::min(mActiveIndex, i);
                size_t end = std::max(mActiveIndex, i);
                
                for (size_t j = start; j <= end; ++j)
                    mSelectedIndices.insert(j);
            }
        }
        else // 単一選択
        {
            if (mSelectedIndices.count(i) == 0)
            {
                mSelectedIndices.clear();
                mSelectedIndices.insert(i);
            }
            mCodes.begin_modify();
            mCodes[i].enabled = !mCodes[i].enabled;
            mCodes.end_modify();
        }

        mActiveIndex = i;
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        mActiveIndex = i;
        if (mSelectedIndices.size() <= 1)
        {
            mSelectedIndices.clear();
            mSelectedIndices.insert(i);
        }
        ImGui::OpenPopup("RightClickMenu");
    }
}

void CodesTab::CodesFrame_DragAndDrop(size_t &i)
{
    if (ImGui::BeginDragDropSource())
    {
        std::vector<int> dragIndices(mSelectedIndices.begin(), mSelectedIndices.end());
        std::sort(dragIndices.begin(), dragIndices.end());

        ImGui::SetDragDropPayload("ENTRIES_MOVE", dragIndices.data(), dragIndices.size() * sizeof(int));
        
        if (dragIndices.size() == 1)
            ImGui::Text("Move Entry: %s", mCodes[dragIndices[0]].name.c_str());
        else
            ImGui::Text("Move %zu Entries", dragIndices.size());

        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTRIES_MOVE"))
        {
            const int *srcIndices = static_cast<const int*>(payload->Data);
            size_t count = payload->DataSize / sizeof(int);
            std::vector<int> indices(srcIndices, srcIndices + count);

            std::sort(indices.begin(), indices.end());

            std::vector<CodeEntry> movingEntries;
            for (int idx : indices)
                movingEntries.push_back(mCodes[idx]);

            mCodes.begin_modify();
            for (auto it = indices.rbegin(); it != indices.rend(); ++it)
                mCodes.remove_entry(*it);

            size_t insertPos = i;
            for (size_t j = 0; j < movingEntries.size(); ++j)
                mCodes.insert_entry(insertPos + j, movingEntries[j]);
            mCodes.end_modify();

            mSelectedIndices.clear();
            for (size_t j = 0; j < movingEntries.size(); ++j)
                mSelectedIndices.insert(insertPos + j);
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
            if (!mSelectedIndices.empty())
            {
                size_t selected = *mSelectedIndices.begin();
                mPopupEntry = mCodes[selected];
                mEditTargetIndex = selected;
                mPopupMode = CodePopupMode::Edit;
                ImGui::OpenPopup("CodePopup");
            }
        }
        if (ImGui::MenuItem("Delete"))
        {
            std::vector<size_t> sortedIndices(mSelectedIndices.begin(), mSelectedIndices.end());
            std::sort(sortedIndices.begin(), sortedIndices.end());

            mCodes.begin_modify();
            for (auto it = sortedIndices.rbegin(); it != sortedIndices.rend(); ++it)
            {
                size_t selectedIndex = *it;
                mCodes.remove_entry(selectedIndex);
                mSelectedIndices.erase(selectedIndex);
            }
            mCodes.end_modify();

            mActiveIndex = -1;
        }
        if (ImGui::MenuItem("Duplicate"))
        {
            std::vector<size_t> newIndices;

            mCodes.begin_modify();
            size_t currentIndex = mCodes.size();
            for (size_t selectedIndex : mSelectedIndices)
            {
                mCodes.insert_entry(currentIndex, mCodes[selectedIndex]);
                newIndices.push_back(currentIndex);
                currentIndex++;
            }
            mCodes.end_modify();

            mSelectedIndices.clear();
            for (size_t newIndex : newIndices)
                mSelectedIndices.insert(newIndex);
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
        if (!mFilteredIndices.empty())
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                mSelectedIndices.clear();
                mActiveIndex = -1;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))
            {
                bool newState = !mCodes[*mSelectedIndices.begin()].enabled;

                mCodes.begin_modify();
                for (size_t i : mSelectedIndices)
                    mCodes[i].enabled = newState;
                mCodes.end_modify();
            }

            if (io.KeyCtrl)
            {
                if (ImGui::IsKeyPressed(ImGuiKey_A))
                {
                    mSelectedIndices.clear();
                    for (size_t i : mFilteredIndices)
                        mSelectedIndices.insert(i);
                    mActiveIndex = mFilteredIndices.empty() ? -1 : mFilteredIndices.back();
                }
            }

            if (!mSelectedIndices.empty())
            {
                size_t currentFilteredPos = -1;
                for (size_t j = 0; j < mFilteredIndices.size(); ++j)
                {
                    if (mFilteredIndices[j] == mActiveIndex)
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
                    if (currentFilteredPos >= 0 && currentFilteredPos < mFilteredIndices.size() - 1)
                        newFilteredPos = currentFilteredPos + 1;
                }

                if (newFilteredPos != -1)
                {
                    size_t newIndex = mFilteredIndices[newFilteredPos];
                    mSelectedIndices.clear();
                    mSelectedIndices.insert(newIndex);
                    mActiveIndex = newIndex;

                    float itemHeight = ImGui::GetFrameHeightWithSpacing();
                    float scrollY = ImGui::GetScrollY();
                    float maxScrollY = ImGui::GetScrollMaxY();
                    float windowHeight = ImGui::GetWindowHeight();

                    float itemYPos = itemHeight * newFilteredPos;

                    if (itemYPos < scrollY)
                        ImGui::SetScrollY(itemYPos);
                    else if (itemYPos + itemHeight > scrollY + windowHeight)
                        ImGui::SetScrollY(itemYPos + itemHeight - windowHeight);
                }
            }
        }

        if (io.KeyCtrl)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_F)) // Search
                mShowSearchBar = !mShowSearchBar;
            
            if (ImGui::IsKeyPressed(ImGuiKey_Z)) // Undo
                mCodes.undo();
            
            if (io.KeyShift && ImGui::IsKeyPressed(ImGuiKey_Z)) // Redo
                mCodes.redo();
        }
    }

    for (size_t i = 0; i < mFilteredIndices.size(); ++i)
    {
        size_t index = mFilteredIndices[i];

        ImVec2 fullWidth = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetTextLineHeightWithSpacing());
        bool isSelected = mSelectedIndices.count(index) > 0;

        if (isSelected)
        {
            ImU32 color = ImGui::GetColorU32(ImGuiCol_Header);
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(
                cursorPos,
                ImVec2(cursorPos.x + fullWidth.x, cursorPos.y + ImGui::GetFrameHeight()),
                ImColor(ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]),
                ImGui::GetStyle().FrameRounding
            );
        }

        ImGui::PushID(i);

        ImVec2 checkboxPos = ImGui::GetCursorPos();
        ImGui::InvisibleButton("##EntryWidget", ImVec2(fullWidth.x, ImGui::GetFrameHeight()));

        CodesFrame_MouseClick(io, isSelected, index);
        CodesFrame_DragAndDrop(index);

        std::string label = mCodes[index].name;
        if (!mCodes[index].authors.empty())
            label += std::format(" [{}]", mCodes[index].authors);

        std::string displayLabel = label;

        float maxLabelWidth = fullWidth.x - ImGui::GetStyle().FramePadding.x * 2.0f - ImGui::GetFontSize();
        ImVec2 textSize = ImGui::CalcTextSize(label.c_str());

        bool wasTruncated = false;
        if (textSize.x > maxLabelWidth)
        {
            wasTruncated = true;
            while (displayLabel.length() > 3 &&
                   ImGui::CalcTextSize((displayLabel.substr(0, displayLabel.length() - 3) + "...").c_str()).x > maxLabelWidth)
                displayLabel.pop_back();
            displayLabel = displayLabel.substr(0, displayLabel.length() - 3) + "...";
        }

        ImGui::SetCursorPos(checkboxPos);
        ImGui::Checkbox(displayLabel.c_str(), &mCodes[index].enabled);

        if (wasTruncated && ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", label.c_str());

        CodesFrame_ContextMenu();

        ImGui::PopID();
    }

    ImGui::PopStyleVar();

    if (mListUpdated)
    {
        ImGui::SetScrollHereY(1.0f);
        mListUpdated = false;
    }

    ImGui::EndChild();
}

