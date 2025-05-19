#include "RaimTabManager.h"

#include "IRaimTab.h"
#include "impl/codes/CodesTab.h"
#include "impl/MemoryEditorTab.h"
#include "impl/settings/SettingsTab.h"
#include "impl/LogsTab.h"
#ifdef DEBUG_MODE
#include "impl/DebugTab.h"
#endif
#include <utils/tcp_gecko/TCPGecko.h>
#include <raim/ui/RaimUI.h>
#include <raim/Raim.h>

#include <imgui.h>


RaimTabManager::RaimTabManager(RaimUI *raim_ui)
    : m_raim_ui(raim_ui)
{
    AllocTabs();
}

RaimTabManager::~RaimTabManager()
{
    DeallocTabs();
}

void RaimTabManager::AllocTabs()
{
    m_tabs = {
        new CodesTab(m_raim_ui),
        new MemoryEditorTab(m_raim_ui),
        new SettingsTab(m_raim_ui),
        new LogsTab(m_raim_ui),
#ifdef DEBUG_MODE
        new DebugTab(m_raim_ui),
#endif
    };
}

void RaimTabManager::DeallocTabs()
{
    for (IRaimTab *tab : m_tabs)
    {
        delete tab;
        tab = nullptr;
    }
}

void RaimTabManager::Update() 
{
    bool on_connected = false;
    bool on_disconnected = false;

    std::shared_ptr<TCPGecko> tcp = m_raim_ui->get_raim()->get_tcp_gecko();

    bool connected = tcp->is_connected();
    if (connected != m_last_connected)
    {
        if (connected)
            on_connected = true;
        else
            on_disconnected = true;
        m_last_connected = connected;
    }

    if (ImGui::BeginTabBar("MainTabs"))
    {
        IRaimTab *current_tab = nullptr;

        for (IRaimTab *tab : m_tabs)
        {
            bool opened = ImGui::BeginTabItem(tab->get_tab_name());
            if (opened)
            {
                current_tab = tab;
                ImGui::BeginChild("TabFrame", ImVec2(0, 0), ImGuiChildFlags_Borders, ImGuiWindowFlags_AlwaysUseWindowPadding);
                tab->Update();
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            tab->UpdateBackground();

            if (on_connected)
                tab->OnConnected();
            if (on_disconnected)
                tab->OnDisconnected();
        }

        if (current_tab != m_active_tab)
        {
            if (m_active_tab)
                m_active_tab->OnTabClosed();

            if (current_tab)
                current_tab->OnTabOpened();

            m_active_tab = current_tab;
        }

        ImGui::EndTabBar();
    }
    else
    {
        if (m_active_tab)
        {
            m_active_tab->OnTabClosed();
            m_active_tab = nullptr;
        }
    }
}

IRaimTab *RaimTabManager::find_tab_by_name(const char *tab_name) const
{
    for (IRaimTab *tab : m_tabs)
    {
        if (strcmp(tab->get_tab_name(), tab_name) == 0)
            return tab;
    }
    return nullptr;
}
