#pragma once

#include <string>
#include <memory>

#include <utils/AsyncTask.h>
#include <utils/pretendo_clients/types.h>

#include "../IRaimTab.h"


class PNASClient;

class DebugTab : public IRaimTab
{
private:
    AsyncTask<std::string> m_account_id_task;

    AsyncTask<std::wstring> m_mii_name_task;

    char m_game_mode_description_str[256] = "";
    AsyncTask<std::string> m_set_game_mode_description_task;

    std::shared_ptr<PNASClient> m_pnas;
    AsyncTask<PrincipalId> m_pnas_principal_id_task;
    AsyncTask<std::string> m_pnas_account_id_task;

public:
    DebugTab(RaimUI *raim_ui);

    void Update() override;
    void UpdateBackground() override;
};
