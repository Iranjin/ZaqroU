#include "CodesTab.h"

#include <filesystem>

#include <utils/TCPGecko.h>
#include <raim/ui/NotificationManager.h>
#include "../../../RaimUI.h"

#include <tinyfiledialogs/tinyfiledialogs.h>
#include <imgui.h>


void CodesTab::ControlsFrame(const ImVec2 &available)
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
            saved_file_path = mLoadedPath;
        }

        if (!saved_file_path.empty())
            getRaimUI()->GetNotificationManager()->AddNotification(mNotifTitle, std::format("Saved \"{}\"", saved_file_path));
    }

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

        mListUpdated = true;
    }
    ImGui::EndDisabled();

    ImGui::BeginDisabled(mLoadedPath.empty() || tcp->is_connected());
    ImGui::SameLine();
    if (ImGui::Button("Unload Code List"))
    {
        mCodes.clear();
        mLoadedPath.clear();
        mSelectedIndices.clear();
        mActiveIndex = -1;
    }
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

            mCodes.begin_modify();
            if (ext == ".bin")
                CodeLoader::LoadFromFile(path, mCodes);
            else if (ext == ".xml")
                CodeLoader::LoadFromXmlFile(path, mCodes);
            mCodes.end_modify();

            getRaimUI()->GetNotificationManager()->AddNotification(mNotifTitle, std::format("Imported \"{}\"", path));
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Untick All"))
    {
        mCodes.begin_modify();
        for (CodeEntry &entry : mCodes)
            entry.enabled = false;
        mCodes.end_modify();
    }
    // Line 1 END

    // Line 2 START
    ImGui::BeginDisabled(!tcp->is_connected());
    ImGui::BeginDisabled(!mCodes.has_enabled_entry());
    if (ImGui::Button("Send Codes"))
        SendCodes();
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Disable Codes"))
        DisableCodes();
    ImGui::EndDisabled();
    // Line 2 END

    ImGui::EndChild();
}
