#include "CodesTab.h"

#include <filesystem>
#include <format>

#include <utils/tcp_gecko/TCPGecko.h>
#include <utils/Config.h>
#include <utils/common.h>
#include <raim/ui/NotificationManager.h>
#include "backend/CodeLoader.h"
#include "../../../RaimUI.h"

#include <tinyfiledialogs/tinyfiledialogs.h>
#include <imgui.h>


void CodesTab::ControlsFrame(const ImVec2 &available)
{
    ImGuiIO &io = ImGui::GetIO();
    
    std::filesystem::path titles_path = get_save_dir() / "titles/";
    
    float bottom_height = (available.y - ImGui::GetStyle().ItemSpacing.y) / 3.0f;

    ImGui::BeginChild("ControlsFrame", ImVec2(-FLT_MIN, -FLT_MIN), ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_HorizontalScrollbar);

    std::shared_ptr<TCPGecko> tcp = get_tcp_gecko();
    
    // Line 1 BEGIN
    if (ImGui::Button("Add Code"))
    {
        if (m_code_window_mode == CodeWindowMode::None)
            OpenAddCodeWindow();
        else
            CloseCodeWindow();
    }

    ImGui::SameLine();
    if (ImGui::Button("Save Code List"))
    {
        std::filesystem::path saved_file_path;
        
        if (m_loaded_path.empty())
        {
            std::filesystem::create_directories(titles_path);

            std::string path = show_save_file_dialog(
                "Save file",
                titles_path,
                { "*.bin" },
                ".bin file");

            if (!path.empty())
            {
                CodeLoader::save_to_file(path, m_codes);
                saved_file_path = path;
            }
        }
        else
        {
            SaveCodes();
            saved_file_path = m_loaded_path;
        }

        if (!saved_file_path.empty())
            get_raim_ui()->get_notification_manager()->AddNotification(m_notif_title, std::format("Saved \"{}\"", saved_file_path.string()));
    }

    ImGui::BeginDisabled(tcp->is_connected());
    ImGui::SameLine();
    if (ImGui::Button("Load Code List"))
    {
        std::string path;

        if (io.KeyShift)
        {
            path = get_config()->get_nested("codes.last_opened", std::string());
            if (path.empty())
                get_raim_ui()->get_notification_manager()->AddErrorNotification(m_notif_title, "No previously opened code list found.");
        }
        else
        {
            path = show_open_file_dialog(
                "Select file to load",
                titles_path,
                { "*.bin" },
                ".bin file");
        }

        if (!path.empty())
        {
            LoadCodes(path);
            m_list_updated = true;
        }
    }
    ImGui::EndDisabled();

    ImGui::BeginDisabled(m_loaded_path.empty() || tcp->is_connected());
    ImGui::SameLine();
    if (ImGui::Button("Unload Code List"))
    {
        m_codes.clear();
        m_loaded_path.clear();
        m_selected_indices.clear();
        m_active_index = -1;
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Import Codes"))
    {
        std::filesystem::create_directories(titles_path);

        std::string path = show_open_file_dialog(
            "Select file to import",
            titles_path,
            { "*.bin", "*.xml" },
            "Code file");

        if (!path.empty())
        {
            m_codes.begin_modify();
            LoadCodes(path, false, false);
            m_codes.end_modify();

            SaveCodes(true);

            CodesFrame_ScrollToIndex(0, true);
        }
    }

    bool has_enabled_entry = m_codes.has_enabled_entry();

    ImGui::SameLine();
    ImGui::BeginDisabled(!has_enabled_entry);
    if (ImGui::Button("Untick All"))
    {
        m_codes.begin_modify();
        for (CodeEntry &entry : m_codes)
            entry.enabled = false;
        m_codes.end_modify();
    }
    ImGui::EndDisabled();
    // Line 1 END

    // Line 2 BEGIN
    ImGui::BeginDisabled(!tcp->is_connected());
    ImGui::BeginDisabled(!has_enabled_entry);
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
