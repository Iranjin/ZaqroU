#pragma once

#include <memory>


class RaimUI;
class Raim;
class Config;

class IRaimTab
{
private:
    RaimUI *mRaimUI;
    const char *mTabName;

public:
    IRaimTab(RaimUI *raimUI, const char *tabMame)
        : mRaimUI(raimUI), mTabName(tabMame)
    {
    }
    
    virtual ~IRaimTab() = default;

    virtual void Update() = 0;

    virtual void OnTabOpened() {}
    virtual void OnTabClosed() {}
    virtual void OnConnected() {}
    virtual void OnDisconnected() {}

    const char *getTabName() { return mTabName; }
    RaimUI *getRaimUI() { return mRaimUI; }
    Raim *getRaim();
    std::shared_ptr<Config> getConfig();
};
