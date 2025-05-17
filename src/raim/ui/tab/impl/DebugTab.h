#pragma once

#include <string>

#include <utils/AsyncTask.h>

#include "../IRaimTab.h"


class DebugTab : public IRaimTab
{
private:
    AsyncTask<std::string> m_account_id_task;
    AsyncTask<std::wstring> m_mii_name_task;

public:
    DebugTab(RaimUI *raimUI);

    void Update() override;
    void UpdateBackground() override;
};
