#include "CodesTab.h"

#include <imgui.h>


void CodesTab::CommentFrame()
{
    if (mActiveIndex == -1)
        return;
    
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    ImGui::InputTextMultiline(
        "##CommentText",
        (char*) mCodes[mActiveIndex].comment.c_str(),
        mCodes[mActiveIndex].comment.size() + 1,
        ImVec2(-FLT_MIN, -FLT_MIN),
        ImGuiInputTextFlags_ReadOnly
    );

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
}
