#pragma once

#include "../IRaimUI_Theme.h"


class ZaqroTheme : public IRaimUITheme
{
public:
    ZaqroTheme();

    void Apply() const override;
};
