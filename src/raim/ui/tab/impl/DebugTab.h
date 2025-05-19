#pragma once

#include <string>

#include <utils/AsyncTask.h>

#include "../IRaimTab.h"


class DebugTab : public IRaimTab
{
private:
    AsyncTask<std::string> m_account_id_task;

    AsyncTask<std::wstring> m_mii_name_task;

    char m_game_mode_description_str[256] = "";
    AsyncTask<std::string> m_set_game_mode_description_task;

public:
    DebugTab(RaimUI *raim_ui);

    void Update() override;
    void UpdateBackground() override;
};
