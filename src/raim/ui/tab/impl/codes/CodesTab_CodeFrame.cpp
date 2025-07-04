#include "CodesTab.h"

#include <filesystem>

#include <imgui.h>


void CodesTab::CodeFrame()
{
    if (ImGui::BeginPopupContextItem("CodeFrameContextMenu"))
    {
        if (ImGui::MenuItem("Copy"))
            m_code_frame_editor.Copy();
        ImGui::EndPopup();
    }

    static size_t s_last_active_index = -1;
    if (m_active_index != s_last_active_index || m_selected_entry_updated)
    {
        m_code_frame_editor.SetText(m_codes[m_active_index].codes);
        s_last_active_index = m_active_index;
    }
    
    if (m_active_index != -1)
    {
        m_code_frame_editor.Render("editor");
        
        if (m_code_frame_editor.HasSelection())
        {
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                ImGui::OpenPopup("CodeFrameContextMenu");
        }
    }
}
