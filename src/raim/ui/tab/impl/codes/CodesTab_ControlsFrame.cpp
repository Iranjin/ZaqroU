#include "CodesTab.h"

#include <filesystem>
#include <format>

#include <utils/tcp_gecko/TCPGecko.h>
#include <utils/common.h>
#include <raim/ui/NotificationManager.h>
#include "backend/CodeLoader.h"
#include "../../../RaimUI.h"

#include <tinyfiledialogs/tinyfiledialogs.h>
#include <imgui.h>


void CodesTab::ControlsFrame(const ImVec2 &available)
{
    std::string titles_path = get_save_dir() + "/titles";
    
    float bottom_height = (available.y - ImGui::GetStyle().ItemSpacing.y) / 3.0f;

    ImGui::BeginChild("ControlsFrame", ImVec2(-FLT_MIN, -FLT_MIN), ImGuiChildFlags_Border, ImGuiWindowFlags_AlwaysUseWindowPadding);

    std::shared_ptr<TCPGecko> tcp = get_tcp_gecko();
    
    // Line 1 BEGIN
    if (ImGui::Button("Add Code"))
    {
        if (m_code_window_mode == CodeWindowMode::None)
        {
            m_code_window_entry = CodeEntry();
            m_code_window_mode = CodeWindowMode::Add;
        }
        else
        {
            m_code_window_mode = CodeWindowMode::None;
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Save Code List"))
    {
        std::string saved_file_path;
        
        if (m_loaded_path.empty())
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
            get_raim_ui()->get_notification_manager()->AddNotification(m_notif_title, std::format("Saved \"{}\"", saved_file_path));
    }

    ImGui::BeginDisabled(tcp->is_connected());
    ImGui::SameLine();
    if (ImGui::Button("Load Code List"))
    {
        const char *filters[] = { "*.bin" };
        const char *path = tinyfd_openFileDialog(
            "Select file to load",
            (get_save_dir() + "/titles/").c_str(),
            1, filters,
            ".dat file",
            0);

        if (path)
        {
            m_codes.clear();
            CodeLoader::load_from_file(path, m_codes);
            CodesFrame_ScrollToIndex(0, true);
            m_loaded_path = path;
        }

        m_list_updated = true;
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

        const char *filters[] = { "*.bin", "*.xml" };
        const char *path = tinyfd_openFileDialog(
            "Select file to import",
            (get_save_dir() + "/titles/").c_str(),
            2, filters,
            "Code file",
            0);

        if (path)
        {
            std::string ext = std::filesystem::path(path).extension().string();

            m_codes.begin_modify();
            if (ext == ".xml")
                CodeLoader::load_from_xml_file(path, m_codes);
            else // .bin
                CodeLoader::load_from_file(path, m_codes);
            m_codes.end_modify();

            SaveCodes(true);

            CodesFrame_ScrollToIndex(0, true);

            get_raim_ui()->get_notification_manager()->AddNotification(m_notif_title, std::format("Imported \"{}\"", path));
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
