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
            if (m_popup_mode == CodePopupMode::Edit && m_edit_target_index >= 0 && m_edit_target_index < (int)m_codes.size())
            {
                const CodeEntry& entry = m_codes[m_edit_target_index];
                strncpy(inputTitle, entry.name.c_str(), sizeof(inputTitle));
                inputTitle[sizeof(inputTitle) - 1] = '\0';

                strncpy(inputAuthors, entry.authors.c_str(), sizeof(inputAuthors));
                inputAuthors[sizeof(inputAuthors) - 1] = '\0';

                strncpy(inputCode, entry.codes.c_str(), sizeof(inputCode));
                inputCode[sizeof(inputCode) - 1] = '\0';

                strncpy(inputComment, entry.comment.c_str(), sizeof(inputComment));
                inputComment[sizeof(inputComment) - 1] = '\0';

                inputAssemblyRamWrite = entry.assembly_ram_write;
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
            entry.assembly_ram_write = inputAssemblyRamWrite;
            entry.enabled = m_codes[m_edit_target_index].enabled;
            entry.codes = inputCode;

            m_codes.begin_modify();
            if (m_popup_mode == CodePopupMode::Add)
                m_codes.add_entry(entry);
            else if (m_popup_mode == CodePopupMode::Edit && m_edit_target_index >= 0 && m_edit_target_index < (int)m_codes.size())
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
