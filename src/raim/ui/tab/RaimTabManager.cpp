#include "RaimTabManager.h"

#include "IRaimTab.h"
#include "impl/codes/CodesTab.h"
#include "impl/MemoryEditorTab.h"
// #include "impl/DebugTab.h"
#include "impl/settings/SettingsTab.h"
#include <utils/TCPGecko.h>
#include <raim/ui/RaimUI.h>
#include <raim/Raim.h>

#include <imgui.h>


RaimTabManager::RaimTabManager(RaimUI *raimUI)
    : mRaimUI(raimUI)
{
    AllocTabs();
}

RaimTabManager::~RaimTabManager()
{
    DeallocTabs();
}

void RaimTabManager::AllocTabs()
{
    mTabs = {
        new CodesTab(mRaimUI),
        new MemoryEditorTab(mRaimUI),
        new SettingsTab(mRaimUI),
        // new DebugTab(mRaimUI),
    };
}

void RaimTabManager::DeallocTabs()
{
    for (IRaimTab *tab : mTabs)
    {
        delete tab;
        tab = nullptr;
    }
}

void RaimTabManager::Update() 
{
    bool on_connected = false;
    bool on_disconnected = false;

    std::shared_ptr<TCPGecko> tcp = mRaimUI->getRaim()->getTCPGecko();

    bool connected = tcp->is_connected();
    if (connected != mLastConnected)
    {
        if (connected)
            on_connected = true;
        else
            on_disconnected = true;
        mLastConnected = connected;
    }

    if (ImGui::BeginTabBar("MainTabs"))
    {
        IRaimTab *currentTab = nullptr;

        for (IRaimTab *tab : mTabs)
        {
            bool opened = ImGui::BeginTabItem(tab->getTabName());
            if (opened)
            {
                currentTab = tab;
                ImGui::BeginChild("TabFrame", ImVec2(0, 0), ImGuiChildFlags_Borders, ImGuiWindowFlags_AlwaysUseWindowPadding);
                tab->Update();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            if (on_connected)
                tab->OnConnected();
            if (on_disconnected)
                tab->OnDisconnected();
        }

        if (currentTab != mActiveTab)
        {
            if (mActiveTab)
                mActiveTab->OnTabClosed();

            if (currentTab)
                currentTab->OnTabOpened();

            mActiveTab = currentTab;
        }

        ImGui::EndTabBar();
    }
    else
    {
        if (mActiveTab)
        {
            mActiveTab->OnTabClosed();
            mActiveTab = nullptr;
        }
    }
}
