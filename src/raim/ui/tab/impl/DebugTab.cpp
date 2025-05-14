/*#include "DebugTab.h"

#include <iostream>

#include <utils/TCPGecko.h>
#include <raim/ui/RaimUI.h>
#include <raim/Raim.h>

#include <imgui.h>


DebugTab::DebugTab(RaimUI *raimUI)
    : IRaimTab(raimUI, "Debug")
{
}

void DebugTab::Update()
{
    std::shared_ptr<TCPGecko> tcp = getRaim()->getTCPGecko();

    ImGui::BeginDisabled(!tcp->is_connected());
    
    if (ImGui::Button("DebugButton"))
    {
        
    }
    ImGui::EndDisabled();
}

void DebugTab::OnTabOpened()
{
    mTabOpened = true;
}*/
