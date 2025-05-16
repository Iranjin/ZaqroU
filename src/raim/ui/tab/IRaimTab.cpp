#include "IRaimTab.h"

#include <utils/Config.h>
#include <raim/ui/NotificationManager.h>
#include "../RaimUI.h"
#include "../../Raim.h"


Raim *IRaimTab::getRaim()
{
    return mRaimUI->getRaim();
}

NotificationManager *IRaimTab::getNotificationManager() { return mRaimUI->getNotificationManager(); }
std::shared_ptr<Config> IRaimTab::getConfig() { return mRaimUI->getRaim()->getConfig(); }
