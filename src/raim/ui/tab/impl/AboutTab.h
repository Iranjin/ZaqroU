#pragma once

#include "../IRaimTab.h"


class AboutTab : public IRaimTab
{
public:
    AboutTab(RaimUI *raim_ui);

    void Update() override;
};
