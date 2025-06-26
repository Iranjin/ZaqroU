#include "CodesTab.h"

#include <imgui.h>


void CodesTab::Update()
{
    ImGui::BeginChild("MainScrollArea");
    
    ImVec2 available = ImGui::GetContentRegionAvail();
    CodesTable(available);
    ControlsFrame(available);

    ImGui::EndChild();
}

void CodesTab::UpdateBackground()
{
    CodeWindow();
}
