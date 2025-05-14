#include "CodesTab.h"

#include <imgui.h>


void CodesTab::CodesTable(const ImVec2 &available)
{
    float topHeight = (available.y - ImGui::GetStyle().ItemSpacing.y) * 2.0f / 3.0f;
    
    if (ImGui::BeginTable("CodesTable", 3, 
        ImGuiTableFlags_Borders | 
        ImGuiTableFlags_RowBg | 
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_ScrollY,
        ImVec2(0, topHeight)))
    {
        ImGui::TableNextColumn();
        ImGui::BeginChild("CodesSearchBarFrame", ImVec2(0, 0), ImGuiChildFlags_None);
        CodesFrame_Search();
        ImGui::BeginChild("CodesFrame", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysUseWindowPadding);
        CodesFrame();
        ImGui::EndChild();
        ImGui::EndChild();

        ImGui::TableNextColumn();
        ImGui::BeginChild("CodeFrame", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysUseWindowPadding);
        CodeFrame();
        ImGui::EndChild();

        ImGui::TableNextColumn();
        ImGui::BeginChild("CommentFrame", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysUseWindowPadding);
        CommentFrame();
        ImGui::EndChild();

        ImGui::EndTable();
    }
}