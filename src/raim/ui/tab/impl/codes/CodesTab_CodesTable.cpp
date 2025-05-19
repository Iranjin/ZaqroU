#include "CodesTab.h"

#include <imgui.h>


void CodesTab::CodesTable(const ImVec2 &available)
{
    float top_height = (available.y - ImGui::GetStyle().ItemSpacing.y) * 2.0f / 3.0f;
    
    if (ImGui::BeginTable("CodesTable", 3, 
        ImGuiTableFlags_Borders | 
        ImGuiTableFlags_RowBg | 
        ImGuiTableFlags_Resizable | 
        ImGuiTableFlags_ScrollY,
        ImVec2(0, top_height)))
    {
        ImGui::TableNextColumn();
        ImGui::BeginChild("CodesSearchBarFrame");
        CodesFrame_Search();
        ImGui::BeginChild("CodesFrame");
        CodesFrame();
        ImGui::EndChild();
        ImGui::EndChild();

        ImGui::TableNextColumn();
        ImGui::BeginChild("CodeFrame");
        CodeFrame();
        ImGui::EndChild();

        ImGui::TableNextColumn();
        ImGui::BeginChild("CommentFrame");
        CommentFrame();
        ImGui::EndChild();

        ImGui::EndTable();
    }
}