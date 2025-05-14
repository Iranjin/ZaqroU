#ifdef DEBUG_MODE
#include "DebugTab.h"

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
        auto tmp = tcp->get_server_version();
        std::cout << tmp << std::endl;
    }

    ImGui::EndDisabled();
}
#endif
