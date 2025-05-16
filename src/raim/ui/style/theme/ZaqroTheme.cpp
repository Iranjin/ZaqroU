#include "ZaqroTheme.h"

#include <imgui.h>


ZaqroTheme::ZaqroTheme()
    : IRaimUITheme("Zaqro")
{
    SetDisplayName(GetDisplayName() + " (Default)");
}

void ZaqroTheme::Apply() const
{
    IRaimUITheme::Apply();
}
