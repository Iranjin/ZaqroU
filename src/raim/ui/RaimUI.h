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

    Raim *get_raim() const { return m_raim; }
    RaimUI_Theme *get_ui_theme() const { return m_ui_theme; }
    NotificationManager *get_notification_manager() const { return m_notif_manager; }
    RaimTabManager *get_tab_manager() const { return m_tab_manager; }
    std::shared_ptr<Config> get_config() const;

    void set_allow_disconnect(bool allow) { m_is_disconnect_allowed = allow; }
};
