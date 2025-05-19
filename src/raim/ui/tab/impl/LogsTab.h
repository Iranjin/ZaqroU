#pragma once

#include "../IRaimTab.h"


class LogsTab : public IRaimTab
{
public:
    LogsTab(RaimUI *raim_ui);

    void Update() override;
};
