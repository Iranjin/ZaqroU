#pragma once

#include "../IRaimUI_Theme.h"


class PinkTheme : public IRaimUITheme
{
public:
    PinkTheme();

    void apply() const override;
};
