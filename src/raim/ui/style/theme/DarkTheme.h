#pragma once

#include "../IRaimUI_Theme.h"


class DarkTheme : public IRaimUITheme
{
public:
    DarkTheme();

    void apply() const override;
};
