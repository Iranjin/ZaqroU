#pragma once

#include <vector>


class RaimUI;
class IRaimTab;

class RaimTabManager
{
private:
    RaimUI *mRaimUI;
    std::vector<IRaimTab*> mTabs;
    IRaimTab* mActiveTab;

    bool mConnected = false,
         mLastConnected = false;

    void AllocTabs();
    void DeallocTabs();

public:
    RaimTabManager(RaimUI *raimUI);
    ~RaimTabManager();

    void Update();
};
