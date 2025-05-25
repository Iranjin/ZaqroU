#include "CodesTab.h"

#include <imgui.h>


void CodesTab::CodeWindow()
{
    if (m_code_window_mode == CodeWindowMode::None)
        return;

    static bool initialized = false;

    static char input_title[64] = "";
    static char input_authors[64] = "";
    static char input_code[262144] = "";
    static char input_comment[131072] = "";
    static bool input_assembly_ram_write = false;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

    if (!initialized)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(500, 600));
    }

    ImGui::Begin("Code", (bool*)&m_code_window_mode);

    if (!initialized)
    {
        if (m_code_window_mode == CodeWindowMode::Edit && m_edit_target_index >= 0 && m_edit_target_index < (int)m_codes.size())
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

    // Title
    ImGui::TextUnformatted("Title");
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::InputText("##Title", input_title, IM_ARRAYSIZE(input_title));

    // Author(s)
    ImGui::TextUnformatted("Author(s)");
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::InputText("##Authors", input_authors, IM_ARRAYSIZE(input_authors));

    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    float avail = ImGui::GetContentRegionAvail().y;
    float line_height = ImGui::GetFrameHeightWithSpacing();
    float fixed_height = line_height * 2 + ImGui::GetStyle().WindowPadding.y + line_height;

    float multiline_height = avail - fixed_height;
    if (multiline_height < 100.0f) multiline_height = 100.0f;

    ImGui::Columns(2, "CodeWindowFields", false);
    ImGui::TextUnformatted("Code");
    ImGui::InputTextMultiline("##Code", input_code, IM_ARRAYSIZE(input_code), ImVec2(-FLT_MIN, multiline_height));
    ImGui::NextColumn();
    ImGui::TextUnformatted("Comment");
    ImGui::InputTextMultiline("##Comment", input_comment, IM_ARRAYSIZE(input_comment), ImVec2(-FLT_MIN, multiline_height));
    ImGui::Columns(1);

    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    // ImGui::NextColumn();
    ImGui::Checkbox("Assembly RAM Writes", &input_assembly_ram_write);
    // ImGui::NextColumn();
    // static bool s_display_code_wizard = false;
    // if (ImGui::Button("Code Wizard", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
    //     s_display_code_wizard = !s_display_code_wizard;
    // if (s_display_code_wizard)
    // {
    //     ImGui::Begin("Code Wizard", &s_display_code_wizard);
        
    //     ImGui::End();
    // }
    // ImGui::Columns(1);

    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    if (ImGui::Button("OK", ImVec2(-FLT_MIN, 0)))
    {
        CodeEntry entry;
        entry.name = input_title;
        entry.authors = input_authors;
        entry.comment = input_comment;
        entry.assembly_ram_write = input_assembly_ram_write;
        if (m_code_window_mode == CodeWindowMode::Edit)
            entry.enabled = m_codes[m_edit_target_index].enabled;
        entry.codes = input_code;

        m_codes.begin_modify();
        if (m_code_window_mode == CodeWindowMode::Add)
            m_codes.add_entry(entry);
        else if (m_code_window_mode == CodeWindowMode::Edit && m_edit_target_index >= 0 && m_edit_target_index < m_codes.size())
            m_codes[m_edit_target_index] = entry;
        m_codes.end_modify();

        initialized = false;
        m_code_window_mode = CodeWindowMode::None;
    }

    ImGui::End();

    ImGui::PopStyleVar();
}
