#include "IRaimTab.h"

#include <utils/Config.h>
#include <raim/ui/NotificationManager.h>
#include "../RaimUI.h"
#include "../../Raim.h"


Raim *IRaimTab::get_raim()
{
    return m_raim_ui->get_raim();
}

NotificationManager *IRaimTab::get_notification_manager() { return m_raim_ui->get_notification_manager(); }
std::shared_ptr<Config> IRaimTab::get_config() { return m_raim_ui->get_raim()->get_config(); }
