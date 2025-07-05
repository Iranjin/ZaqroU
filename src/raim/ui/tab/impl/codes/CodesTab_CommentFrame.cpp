#include "CodesTab.h"

#include <imgui.h>
#include <imgui_markdown.h>


void CodesTab::CommentFrame()
{
    if (m_active_index == -1)
        return;

    ImGui::MarkdownConfig config;
    
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));

    if (ImGui::BeginChild("##CommentMarkdown"))
    {
        const std::string &comment_text = m_codes[m_active_index].comment;
        ImGui::Markdown(comment_text.c_str(), comment_text.length(), config);
    }
    ImGui::EndChild();

    ImGui::PopStyleColor();
}
