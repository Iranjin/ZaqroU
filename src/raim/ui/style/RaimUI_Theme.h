#pragma once

#include <vector>

#include "IRaimUI_Theme.h"

#include "theme/ZaqroTheme.h"
#include "theme/DarkTheme.h"
#include "theme/GrayScaleTheme.h"
#include "theme/JavaTheme.h"


class RaimUI_Theme
{
private:
    std::vector<IRaimUITheme*> mThemes;
    
public:
    RaimUI_Theme()
    {
        mThemes = {
            new ZaqroTheme(),
            new DarkTheme(),
            new GrayScaleTheme(),
            new JavaTheme(),
        };
    }

    ~RaimUI_Theme()
    {
        for (IRaimUITheme *theme : mThemes)
        {
            delete theme;
            theme = nullptr;
        }
    }

    const std::vector<IRaimUITheme*> &AllThemes()
    {
        return mThemes;
    }

    const IRaimUITheme *FromName(const std::string &name)
    {
        for (IRaimUITheme *theme : AllThemes())
        {
            if (theme->GetName() == name)
                return theme;
        }
        return AllThemes()[0];
    }
};
