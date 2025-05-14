#pragma once

#include "../IRaimTab.h"


class LogsTab : public IRaimTab
{
public:
    LogsTab(RaimUI *raimUI);

    void Update() override;
};
