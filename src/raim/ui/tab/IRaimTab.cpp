#include "IRaimTab.h"

#include <utils/Config.h>
#include "../RaimUI.h"
#include "../../Raim.h"


Raim *IRaimTab::getRaim()
{
    return mRaimUI->getRaim();
}

std::shared_ptr<Config> IRaimTab::getConfig()
{
    return mRaimUI->getRaim()->getConfig();
}
