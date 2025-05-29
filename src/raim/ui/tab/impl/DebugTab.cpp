#include "DebugTab.h"

#include <iostream>

#include <utils/tcp_gecko/TCPGecko.h>
#include <utils/StrUtils.h>
#include <utils/pretendo_clients/PNASClient.h>
#include <raim/ui/NotificationManager.h>
#include <raim/ui/RaimUI.h>
#include <raim/Raim.h>

#include <imgui.h>


DebugTab::DebugTab(RaimUI *raimUI)
    : IRaimTab(raimUI, "Debug"),
      m_pnas(std::make_shared<PNASClient>())
{
}

void DebugTab::Update()
{
    static char s_get_principal_id_by_account_id[32] = "";
    if (ImGui::InputText("get_principal_id_by_account_id",
                         s_get_principal_id_by_account_id,
                         IM_ARRAYSIZE(s_get_principal_id_by_account_id),
                         ImGuiInputTextFlags_EnterReturnsTrue) && 
        !m_pnas_principal_id_task.is_running())
    {
        m_pnas_principal_id_task.run([&]() {
            return m_pnas->get_principal_id(s_get_principal_id_by_account_id);
        });
    }

    static char s_get_account_id_by_account_id[32] = "";
    if (ImGui::InputText("get_account_id_by_account_id",
                         s_get_account_id_by_account_id,
                         IM_ARRAYSIZE(s_get_account_id_by_account_id),
                         ImGuiInputTextFlags_EnterReturnsTrue) && 
        !m_pnas_account_id_task.is_running())
    {
        m_pnas_account_id_task.run([&]() {
            return m_pnas->get_account_id(std::stoul(s_get_account_id_by_account_id));
        });
    }
    
    std::shared_ptr<TCPGecko> tcp = get_raim()->get_tcp_gecko();

    ImGui::BeginDisabled(!tcp->is_connected());

    ImGui::BeginDisabled(m_account_id_task.is_running() || m_mii_name_task.is_running() || m_set_game_mode_description_task.is_running());
    if (ImGui::Button("GetAccountId") && !m_account_id_task.is_running())
    {
        m_account_id_task.run([tcp]() {
            return tcp->get_account_id();
        });
        get_raim_ui()->set_allow_disconnect(false);
    }
    if (ImGui::Button("GetMiiName") && !m_mii_name_task.is_running())
    {
        m_mii_name_task.run([tcp]() {
            return tcp->get_mii_name();
        });
        get_raim_ui()->set_allow_disconnect(false);
    }
    
    if (ImGui::InputText("SetGameModeDescription",
                         m_game_mode_description_str,
                         IM_ARRAYSIZE(m_game_mode_description_str),
                         ImGuiInputTextFlags_EnterReturnsTrue) && 
        !m_set_game_mode_description_task.is_running())
    {
        m_set_game_mode_description_task.run([tcp, description = m_game_mode_description_str]() {
            tcp->set_game_mode_description(string_to_wstring(std::string(description)));
            return description;
        });
        get_raim_ui()->set_allow_disconnect(false);
    }
    // if (ImGui::Button("Debug") && !m_debug_task.is_running())
    // {
    //     m_debug_task.run([tcp]() {
    //         tcp->set_game_mode_description(L"Playing Zaqro U");
    //         return "Success";
    //     });
    //     getRaimUI()->set_allow_disconnect(false);
    // }
    if (ImGui::Button("Shutdown"))
    {
        tcp->shutdown();
        tcp->disconnect();
    }
    if (ImGui::Button("Reboot"))
    {
        tcp->reboot();
        tcp->disconnect();
    }
    ImGui::EndDisabled();

    ImGui::EndDisabled();
}

void DebugTab::UpdateBackground()
{
    if (std::optional<PrincipalId> pid = m_pnas_principal_id_task.get_result())
    {
        get_notification_manager()->AddNotification("DebugTab", std::format("PrincipalId: {}", *pid), 7.5f,
            [pid]() {
                ImGui::SetClipboardText(std::format("{}", *pid).c_str());
            });
    }
    if (std::optional<std::string> account_id = m_pnas_account_id_task.get_result())
    {
        get_notification_manager()->AddNotification("DebugTab", std::format("AccountId: {}", *account_id), 7.5f,
            [account_id]() {
                ImGui::SetClipboardText(account_id->c_str());
            });
    }
    
    if (std::optional<std::string> result = m_account_id_task.get_result())
    {
        get_notification_manager()->AddNotification("DebugTab", "AccountId: " + *result, 7.5f,
            [result]() {
                ImGui::SetClipboardText(result->c_str());
            });

        get_raim_ui()->set_allow_disconnect(true);
    }
    if (std::optional<std::wstring> result = m_mii_name_task.get_result())
    {
        try
        {
            std::string str = wstring_to_string(*result);
            
            get_notification_manager()->AddNotification("DebugTab", "MiiName: " + str, 7.5f,
                [str]() {
                    ImGui::SetClipboardText(str.c_str());
                });
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        get_raim_ui()->set_allow_disconnect(true);
    }
    if (std::optional<std::string> result = m_set_game_mode_description_task.get_result())
    {
        try
        {
            get_notification_manager()->AddNotification("DebugTab", *result, 7.5f,
                [result]() {
                    ImGui::SetClipboardText(result->c_str());
                });
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        get_raim_ui()->set_allow_disconnect(true);
    }
}
