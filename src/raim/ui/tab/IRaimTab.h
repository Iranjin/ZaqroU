#pragma once

#include <memory>


class RaimUI;
class Raim;
class Config;
class NotificationManager;

class IRaimTab
{
private:
    RaimUI *m_raim_ui;
    const char *m_tab_name;

public:
    IRaimTab(RaimUI *raim_ui, const char *tab_name)
        : m_raim_ui(raim_ui), m_tab_name(tab_name)
    {
    }
    
    virtual ~IRaimTab() = default;

    virtual void Update() = 0;
    virtual void UpdateBackground() {};
    virtual void OnTabOpened() {}
    virtual void OnTabClosed() {}
    virtual void OnConnected() {}
    virtual void OnDisconnected() {}

    const char *get_tab_name() { return m_tab_name; }
    RaimUI *get_raim_ui() { return m_raim_ui; }
    Raim *get_raim();
    NotificationManager *get_notification_manager();
    std::shared_ptr<Config> get_config();
};
