#pragma once

#include <memory>
#include <string>


class Config;
class Raim;
class RaimTabManager;
class RaimUI_Theme;
class NotificationManager;
class TitleIdParser;

class RaimUI
{
private:
    Raim *m_raim;
    RaimTabManager *m_tab_manager;
    RaimUI_Theme *m_ui_theme;
    bool m_initialized = false;
    bool m_is_disconnect_allowed = true;

    NotificationManager *m_notif_manager;
    
    // TitleIdParser *m_title_id_parser;

    bool m_show_error_popup = false;
    std::string m_error_message;
    
public:
    RaimUI(Raim *app_instance);
    ~RaimUI();

    void Init();
    void Update();
    void MainUI();

    Raim *getRaim() const { return m_raim; }
    RaimUI_Theme *getUITheme() const { return m_ui_theme; }
    // TitleIdParser *getTitleIdParser() const { return mTitleIdParser; }
    NotificationManager *getNotificationManager() const { return m_notif_manager; }
    std::shared_ptr<Config> getConfig();

    void set_allow_disconnect(bool allow) { m_is_disconnect_allowed = allow; }
};
