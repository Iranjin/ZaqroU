#include "CodesTab.h"

#include <imgui.h>


void CodesTab::CodePopup()
{
    if (m_popup_mode != CodePopupMode::None)
        ImGui::OpenPopup("CodePopup");
    else
        return;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, -FLT_MIN));

    if (ImGui::BeginPopupModal("CodePopup", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
    {
        static char input_title[64] = "";
        static char input_authors[64] = "";
        static char input_code[262144] = "";
        static char input_comment[131072] = "";
        static bool input_assembly_ram_write = false;

        static bool initialized = false;
        if (!initialized)
        {
            if (m_popup_mode == CodePopupMode::Edit && m_edit_target_index >= 0 && m_edit_target_index < (int)m_codes.size())
            {
                const CodeEntry& entry = m_codes[m_edit_target_index];
                strncpy(input_title, entry.name.c_str(), sizeof(input_title));
                input_title[sizeof(input_title) - 1] = '\0';

                strncpy(input_authors, entry.authors.c_str(), sizeof(input_authors));
                input_authors[sizeof(input_authors) - 1] = '\0';

                strncpy(input_code, entry.codes.c_str(), sizeof(input_code));
                input_code[sizeof(input_code) - 1] = '\0';

                strncpy(input_comment, entry.comment.c_str(), sizeof(input_comment));
                input_comment[sizeof(input_comment) - 1] = '\0';

                input_assembly_ram_write = entry.assembly_ram_write;
            }
            else
            {
                input_title[0] = '\0';
                input_authors[0] = '\0';
                input_code[0] = '\0';
                input_comment[0] = '\0';
                input_assembly_ram_write = false;
            }
            initialized = true;
        }

        ImGui::TextUnformatted("Title");
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("##Title", input_title, IM_ARRAYSIZE(input_title));

        ImGui::TextUnformatted("Author(s)");
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::InputText("##Authors", input_authors, IM_ARRAYSIZE(input_authors));

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        ImGui::Columns(2, "EntryFields", false);
        ImGui::TextUnformatted("Code");
        ImGui::InputTextMultiline("##Code", input_code, IM_ARRAYSIZE(input_code), ImVec2(-FLT_MIN, 300));
        ImGui::NextColumn();
        ImGui::TextUnformatted("Comment");
        ImGui::InputTextMultiline("##Comment", input_comment, IM_ARRAYSIZE(input_comment), ImVec2(-FLT_MIN, 300));
        ImGui::Columns(1);

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        ImGui::Checkbox("Assembly RAM Writes", &input_assembly_ram_write);

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        
        if (ImGui::Button("OK", ImVec2(-FLT_MIN, 0)))
        {
            CodeEntry entry;
            entry.name = input_title;
            entry.authors = input_authors;
            entry.comment = input_comment;
            entry.assembly_ram_write = input_assembly_ram_write;
            if (m_popup_mode == CodePopupMode::Edit)
                entry.enabled = m_codes[m_edit_target_index].enabled;
            entry.codes = input_code;

            m_codes.begin_modify();
            if (m_popup_mode == CodePopupMode::Add)
                m_codes.add_entry(entry);
            else if (m_popup_mode == CodePopupMode::Edit && m_edit_target_index >= 0 && m_edit_target_index < m_codes.size())
                m_codes[m_edit_target_index] = entry;
            m_codes.end_modify();

            initialized = false;
            m_popup_mode = CodePopupMode::None;
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
            m_popup_mode = CodePopupMode::None;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::PopStyleVar();
}
