#pragma once

#include "../IRaimUI_Theme.h"


class GrayScaleTheme : public IRaimUITheme
{
public:
    GrayScaleTheme();

    void Apply() const override;
};
