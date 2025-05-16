#pragma once

#include "../IRaimUI_Theme.h"


class DarkTheme : public IRaimUITheme
{
public:
    DarkTheme();

    void Apply() const override;
};
