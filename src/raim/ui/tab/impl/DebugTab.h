#ifdef DEBUG_MODE
#pragma once

#include "../IRaimTab.h"


class DebugTab : public IRaimTab
{
public:
    DebugTab(RaimUI *raimUI);

    void Update() override;
};
#endif
