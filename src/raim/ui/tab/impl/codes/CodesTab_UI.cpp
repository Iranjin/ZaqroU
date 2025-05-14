#include "CodesTab.h"

#include <filesystem>
#include <iostream>

#include <utils/TitleIdParser.h>
#include <utils/TCPGecko.h>
#include <utils/Config.h>
#include <raim/ui/NotificationManager.h>
#include "../../../RaimUI.h"
#include "../../../../Raim.h"

#include <tinyfiledialogs/tinyfiledialogs.h>
#include <tinyxml2.h>
#include <imgui.h>
#include <imgui_internal.h>


void CodesTab::CodePopup()
{
    if (mPopupMode != CodePopupMode::None)
        ImGui::OpenPopup("CodePopup");
    else
        return;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600, -FLT_MIN));

    if (ImGui::BeginPopupModal("CodePopup", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
    {
        static char inputTitle[64] = "";
        static char inputAuthors[64] = "";
        static char inputCode[262144] = "";
        static char inputComment[131072] = "";
        static bool inputAssemblyRamWrite = false;

        static bool initialized = false;
        if (!initialized)
        {
            if (mPopupMode == CodePopupMode::Edit && mEditTargetIndex >= 0 && mEditTargetIndex < (int)mCodes.size())
            {
                const CodeEntry& entry = mCodes[mEditTargetIndex];
                strncpy(inputTitle, entry.name.c_str(), sizeof(inputTitle));
                inputTitle[sizeof(inputTitle) - 1] = '\0';

                strncpy(inputAuthors, entry.authors.c_str(), sizeof(inputAuthors));
                inputAuthors[sizeof(inputAuthors) - 1] = '\0';

                strncpy(inputCode, entry.codes.c_str(), sizeof(inputCode));
                inputCode[sizeof(inputCode) - 1] = '\0';

                strncpy(inputComment, entry.comment.c_str(), sizeof(inputComment));
                inputComment[sizeof(inputComment) - 1] = '\0';

                inputAssemblyRamWrite = entry.assemblyRamWrite;
            }
            else
            {
                inputTitle[0] = '\0';
                inputAuthors[0] = '\0';
                inputCode[0] = '\0';
                inputComment[0] = '\0';
                inputAssemblyRamWrite = false;
            }
            initialized = true;
        }

        ImGui::TextUnformatted("Title");
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("##Title", inputTitle, IM_ARRAYSIZE(inputTitle));

        ImGui::TextUnformatted("Author(s)");
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("##Authors", inputAuthors, IM_ARRAYSIZE(inputAuthors));

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        ImGui::Columns(2, "EntryFields", false);
        ImGui::TextUnformatted("Code");
        ImGui::InputTextMultiline("##Code", inputCode, IM_ARRAYSIZE(inputCode), ImVec2(-FLT_MIN, 400));
        ImGui::NextColumn();
        ImGui::TextUnformatted("Comment");
        ImGui::InputTextMultiline("##Comment", inputComment, IM_ARRAYSIZE(inputComment), ImVec2(-FLT_MIN, 400));
        ImGui::Columns(1);

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        ImGui::Checkbox("Assembly RAM Writes", &inputAssemblyRamWrite);

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        
        if (ImGui::Button("OK", ImVec2(-FLT_MIN, 0)))
        {
            CodeEntry entry;
            entry.name = inputTitle;
            entry.authors = inputAuthors;
            entry.comment = inputComment;
            entry.assemblyRamWrite = inputAssemblyRamWrite;
            entry.enabled = mCodes[mEditTargetIndex].enabled;
            entry.codes = inputCode;

            if (mPopupMode == CodePopupMode::Add)
            {
                mCodes.addCodeEntry(entry);
            }
            else if (mPopupMode == CodePopupMode::Edit && mEditTargetIndex >= 0 && mEditTargetIndex < (int)mCodes.size())
            {
                mCodes[mEditTargetIndex] = entry;
            }

            initialized = false;
            mPopupMode = CodePopupMode::None;
            ImGui::CloseCurrentPopup();
        }

        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 windowEnd = ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y);

        ImVec2 mousePos = ImGui::GetIO().MouseClickedPos[0];

        bool clickedOutside =
            (mousePos.x < windowPos.x || mousePos.x > windowEnd.x ||
            mousePos.y < windowPos.y || mousePos.y > windowEnd.y) &&
            ImGui::IsMouseClicked(0);

        if (clickedOutside)
        {
            initialized = false;
            mPopupMode = CodePopupMode::None;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
}

void CodesTab::CommentFrame()
{
    if (mActiveIndex == -1)
        return;
    
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    ImGui::InputTextMultiline(
        "##CommentText",
        (char*) mCodes[mActiveIndex].comment.c_str(),
        mCodes[mActiveIndex].comment.size() + 1,
        ImVec2(-FLT_MIN, -FLT_MIN),
        ImGuiInputTextFlags_ReadOnly
    );

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}

void CodesTab::ControlsFrame(ImVec2 &available)
{
    std::string titles_path = "zaqro_u/titles";
    
    float bottomHeight = (available.y - ImGui::GetStyle().ItemSpacing.y) / 3.0f;

    ImGui::BeginChild("ControlsFrame", ImVec2(-FLT_MIN, -FLT_MIN), ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysUseWindowPadding);

    std::shared_ptr<TCPGecko> tcp = getTCPGecko();
    
    // Line 1 START
    if (ImGui::Button("Add Code"))
    {
        mPopupEntry = CodeEntry();
        mPopupMode = CodePopupMode::Add;
        ImGui::OpenPopup("CodePopup");
    }

    // ImGui::BeginDisabled(mLoadedPath.empty());
    ImGui::SameLine();
    if (ImGui::Button("Save Code List"))
    {
        std::string saved_file_path;
        
        if (mLoadedPath.empty())
        {
            std::filesystem::create_directories(titles_path);

            const char *filters[] = { "*.bin" };
            const char *path = tinyfd_saveFileDialog(
                "Save file",
                titles_path.c_str(),
                1, filters,
                ".bin file"
            );

            if (path)
            {
                CodeLoader::SaveToFile(path, mCodes);
                saved_file_path = path;
            }
        }
        else
        {
            std::string folder_path = std::filesystem::path(mLoadedPath).parent_path();
            std::filesystem::create_directories(folder_path);
            CodeLoader::SaveToFile(mLoadedPath, mCodes);
            // CodeLoader::SaveToFile("zaqro_u/debug.bin", mCodes);
            saved_file_path = mLoadedPath;
        }

        if (!saved_file_path.empty())
            getRaimUI()->GetNotificationManager()->AddNotification(mNotifTitle, std::format("Saved \"{}\"", saved_file_path));
    }
    // ImGui::EndDisabled();

    ImGui::BeginDisabled(tcp->is_connected());
    ImGui::SameLine();
    if (ImGui::Button("Load Code List"))
    {
        const char *filters[] = { "*.bin" };
        const char *path = tinyfd_openFileDialog(
            "Select file to load",
            "zaqro_u/titles/",
            1, filters,
            ".dat file",
            0);

        if (path)
        {
            mCodes.clear();
            CodeLoader::LoadFromFile(path, mCodes);
            mLoadedPath = path;
        }
        // mCodes.clear();
        // CodeLoader::LoadFromFile("zaqro_u/debug.bin", mCodes);
        // CodeLoader::LoadFromXmlFile("zaqro_u/kuroha.xml", mCodes);

        mListUpdated = true;
    }
    ImGui::EndDisabled();

    ImGui::BeginDisabled(mLoadedPath.empty() || tcp->is_connected());
    // ImGui::BeginDisabled(tcp->is_connected());
    ImGui::SameLine();
    if (ImGui::Button("Unload Code List"))
    {
        mCodes.clear();
        mLoadedPath.clear();
        mSelectedIndices.clear();
        mActiveIndex = -1;
    }
    // ImGui::EndDisabled();
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Import Codes"))
    {
        std::filesystem::create_directories(titles_path);

        const char *filters[] = { "*.bin", "*.xml" };
        const char *path = tinyfd_openFileDialog(
            "Select file to import",
            "zaqro_u/titles/",
            2, filters,
            "Code file",
            0);

        if (path)
        {
            std::filesystem::path filepath = path;
            std::string ext = filepath.extension().string();

            if (ext == ".bin")
                CodeLoader::LoadFromFile(path, mCodes);
            else if (ext == ".xml")
                CodeLoader::LoadFromXmlFile(path, mCodes);

            getRaimUI()->GetNotificationManager()->AddNotification(mNotifTitle, std::format("Imported \"{}\"", path));
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Untick All"))
    {
        for (CodeEntry &entry : mCodes)
            entry.enabled = false;
    }
    // Line 1 END

    // Line 2 START
    ImGui::BeginDisabled(!tcp->is_connected());
    ImGui::BeginDisabled(!mCodes.hasEnabledEntry());
    if (ImGui::Button("Send Codes"))
    {
        SendCodes();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Disable Codes"))
    {
        DisableCodes();
    }
    ImGui::EndDisabled();
    // Line 2 END

    ImGui::EndChild();
}

void CodesTab::Update()
{
    ImVec2 available = ImGui::GetContentRegionAvail();
    
    ImGui::BeginChild("MainScrollArea", ImVec2(0, 0), ImGuiChildFlags_None);
    
    float topHeight = (available.y - ImGui::GetStyle().ItemSpacing.y) * 2.0f / 3.0f;

    // int columns = 3;
    
    if (ImGui::BeginTable("CodesTable", 3, 
        ImGuiTableFlags_Borders | 
        ImGuiTableFlags_RowBg | 
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_ScrollY,
        ImVec2(0, topHeight)))
    {
        ImGui::TableNextColumn();
        ImGui::BeginChild("CodesSearchBarFrame", ImVec2(0, 0), ImGuiChildFlags_None);
        CodesFrame_Search();
        ImGui::BeginChild("CodesFrame", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysUseWindowPadding);
        CodesFrame();
        ImGui::EndChild();
        ImGui::EndChild();

        ImGui::TableNextColumn();
        ImGui::BeginChild("CodeFrame", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysUseWindowPadding);
        CodeFrame();
        ImGui::EndChild();

        ImGui::TableNextColumn();
        ImGui::BeginChild("CommentFrame", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysUseWindowPadding);
        CommentFrame();
        ImGui::EndChild();

        ImGui::EndTable();
    }

    ControlsFrame(available);
    
    ImGui::EndChild();

    CodePopup();
}
