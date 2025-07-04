#include "CodesTab.h"

#include <imgui.h>

#include <utils/common.h>


void CodesTab::CodeWindow()
{
    std::string window_title = "null";

    switch (m_code_window_mode)
    {
    case CodeWindowMode::Add: window_title = "Add Code"; break;
    case CodeWindowMode::Edit: window_title = "Edit Code"; break;
    default: return;
    }

    static char input_title[64] = "";
    static char input_authors[64] = "";
    static char input_comment[131072] = "";
    static bool input_assembly_ram_write = false;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

    if (!m_code_window_initialized)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(500, 600));
    }
    ImGui::Begin(window_title.c_str(), (bool *) &m_code_window_mode);

    if (!m_code_window_initialized)
    {
        if (m_code_window_mode == CodeWindowMode::Edit && m_edit_target_index >= 0 && m_edit_target_index < (int) m_codes.size())
        {
            const CodeEntry &entry = m_codes[m_edit_target_index];
            strncpy(input_title, entry.name.c_str(), sizeof(input_title));
            input_title[sizeof(input_title) - 1] = '\0';

            strncpy(input_authors, entry.authors.c_str(), sizeof(input_authors));
            input_authors[sizeof(input_authors) - 1] = '\0';

            m_code_window_editor.SetText(entry.codes);

            strncpy(input_comment, entry.comment.c_str(), sizeof(input_comment));
            input_comment[sizeof(input_comment) - 1] = '\0';

            input_assembly_ram_write = entry.assembly_ram_write;
        }
        else
        {
            input_title[0] = '\0';
            input_authors[0] = '\0';
            m_code_window_editor.SetText("");
            input_comment[0] = '\0';
            input_assembly_ram_write = false;
        }
        m_code_window_initialized = true;
    }

    // タイトル
    ImGui::TextUnformatted("Title");
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::InputText("##Title", input_title, IM_ARRAYSIZE(input_title));

    // 作成者
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
    { // コード
        ImGui::TextUnformatted("Code");

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild("CodeChild", ImVec2(-FLT_MIN, multiline_height), ImGuiChildFlags_Border);
        m_code_window_editor.Render("Code", ImVec2(-FLT_MIN, multiline_height));
        ImGui::EndChild();
        ImGui::PopStyleVar();
    }
    ImGui::NextColumn();
    { // コメント
        ImGui::TextUnformatted("Comment");
        ImGui::InputTextMultiline("##Comment", input_comment, IM_ARRAYSIZE(input_comment), ImVec2(-FLT_MIN, multiline_height));
    }
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
        {
            std::string codes = m_code_window_editor.GetText();
            codes.pop_back(); // 改行削除
            entry.codes = codes;
        }

        m_codes.begin_modify();
        if (m_code_window_mode == CodeWindowMode::Add)
            m_codes.add_entry(entry);
        else if (m_code_window_mode == CodeWindowMode::Edit && m_edit_target_index >= 0 && m_edit_target_index < m_codes.size())
            m_codes[m_edit_target_index] = entry;
        m_codes.end_modify();

        m_code_window_initialized = false;
        m_code_window_mode = CodeWindowMode::None;

        SaveCodes(true);

        m_selected_entry_updated = true;
    }

    ImGui::End();

    ImGui::PopStyleVar();
}

void CodesTab::OpenAddCodeWindow()
{
    m_code_window_entry = CodeEntry();
    m_code_window_mode = CodeWindowMode::Add;
    m_code_window_initialized = false;
}

void CodesTab::OpenEditCodeWindow(size_t index)
{
    m_code_window_entry = m_codes[index];
    m_edit_target_index = index;
    m_code_window_mode = CodeWindowMode::Edit;
    m_code_window_initialized = false;
}

void CodesTab::CloseCodeWindow()
{
    m_edit_target_index = -1;
    m_code_window_mode = CodeWindowMode::None;
    m_code_window_initialized = false;
}
