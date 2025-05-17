#include "DebugTab.h"

#include <iostream>

#include <utils/TCPGecko.h>
#include <utils/StrUtils.h>
#include <raim/ui/NotificationManager.h>
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

    ImGui::BeginDisabled(m_account_id_task.is_running() || m_mii_name_task.is_running());
    if (ImGui::Button("GetAccountId") && !m_account_id_task.is_running())
    {
        m_account_id_task.run([tcp]() {
            return tcp->get_account_id();
        });
        getRaimUI()->set_allow_disconnect(false);
    }
    if (ImGui::Button("GetMiiName") && !m_mii_name_task.is_running())
    {
        m_mii_name_task.run([tcp]() {
            return tcp->get_mii_name();
        });
        getRaimUI()->set_allow_disconnect(false);
    }
    ImGui::EndDisabled();

    ImGui::EndDisabled();
}

void DebugTab::UpdateBackground()
{
    if (std::optional<std::string> result = m_account_id_task.get_result())
    {
        getNotificationManager()->AddNotification("DebugTab", "AccountId: " + *result, 7.5f,
            [result]() {
                ImGui::SetClipboardText(result->c_str());
            });

        getRaimUI()->set_allow_disconnect(true);
    }
    if (std::optional<std::wstring> result = m_mii_name_task.get_result())
    {
        try
        {
            std::string str = wstring_to_string(*result);
            
            getNotificationManager()->AddNotification("DebugTab", "MiiName: " + str, 7.5f,
                [str]() {
                    ImGui::SetClipboardText(str.c_str());
                });
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        getRaimUI()->set_allow_disconnect(true);
    }
}
