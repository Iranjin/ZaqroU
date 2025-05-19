#include "ZaqroTheme.h"

#include <imgui.h>


ZaqroTheme::ZaqroTheme()
    : IRaimUITheme("Zaqro")
{
    set_display_name(get_display_name() + " (Default)");
}

void ZaqroTheme::apply() const
{
    IRaimUITheme::apply();
}
