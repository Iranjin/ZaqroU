#pragma once

#include "../IRaimUI_Theme.h"


class JavaTheme : public IRaimUITheme
{
public:
    JavaTheme();

    void Apply() const override;
};
