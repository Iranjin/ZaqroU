#pragma once

#include "../IRaimUI_Theme.h"


class ImGuiTheme : public IRaimUITheme
{
public:
    ImGuiTheme();

    void Apply() const override;
};
