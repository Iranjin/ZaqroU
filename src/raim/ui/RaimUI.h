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
    Raim *mRaim;
    RaimTabManager *mTabManager;
    RaimUI_Theme *mUITheme;
    bool mInitialized = false;

    NotificationManager *mNotificationManager;
    
    // TitleIdParser *mTitleIdParser;

    bool mShowErrorPopup = false;
    std::string mErrorMessage;
    
public:
    RaimUI(Raim *appInstance);
    ~RaimUI();

    void Init();
    void Update();
    void MainUI();

    Raim *getRaim() const { return mRaim; }
    RaimUI_Theme *getUITheme() const { return mUITheme; }
    // TitleIdParser *getTitleIdParser() const { return mTitleIdParser; }
    NotificationManager *getNotificationManager() const { return mNotificationManager; }
    std::shared_ptr<Config> getConfig();
};
