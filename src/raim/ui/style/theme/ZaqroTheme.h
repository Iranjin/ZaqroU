#pragma once

#include <imgui.h>

#include "../IRaimUI_Theme.h"


class ZaqroTheme : public IRaimUITheme
{
public:
    ZaqroTheme()
        : IRaimUITheme("Zaqro")
    {
        SetDisplayName(GetDisplayName() + " (Default)");
    }

    void Apply() const override
    {
        IRaimUITheme::Apply();
    }
};
