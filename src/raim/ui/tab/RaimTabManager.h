#pragma once

#include <vector>


class RaimUI;
class IRaimTab;

class RaimTabManager
{
private:
    RaimUI *m_raim_ui;
    std::vector<IRaimTab*> m_tabs;
    IRaimTab *m_active_tab = nullptr;

    bool m_last_connected = false;

    void AllocTabs();
    void DeallocTabs();

public:
    RaimTabManager(RaimUI *raim_ui);
    ~RaimTabManager();

    void Update();

    IRaimTab *get_active_tab() const { return m_active_tab; }
};
