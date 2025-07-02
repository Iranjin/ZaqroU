#pragma once

#include <vector>

#include "IRaimUI_Theme.h"

#include "theme/ZaqroTheme.h"
#include "theme/DarkTheme.h"
#include "theme/PinkTheme.h"
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
            new PinkTheme(),
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

    const std::vector<IRaimUITheme*> &all_themes()
    {
        return mThemes;
    }

    const IRaimUITheme *from_name(const std::string &name)
    {
        for (IRaimUITheme *theme : all_themes())
        {
            if (theme->get_name() == name)
                return theme;
        }
        return all_themes()[0];
    }
};
