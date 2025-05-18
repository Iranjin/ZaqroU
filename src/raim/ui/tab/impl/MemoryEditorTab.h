#pragma once

#include "../IRaimTab.h"

#include "MemoryTableView.h"

#include <vector>
#include <mutex>
#include <cstdint>


class MemoryEditorTab : public IRaimTab
{
private:
    std::vector<uint8_t> m_memory;
    uint32_t m_base_address;
    uint32_t m_mem_size;

    MemoryTableView::EViewMode m_view_mode = MemoryTableView::EViewMode::HEX;
    
    bool m_auto_refresh_enabled = false;
    float m_refresh_timer = 0.0f;

    char m_address_input[9] = {};
    char m_value_input[9] = {};

public:
    MemoryEditorTab(RaimUI *raim_ui);
    ~MemoryEditorTab();

    void Update() override;
    void OnConnected() override;
    void OnTabOpened() override;

private:
    std::shared_ptr<MemoryTableView> m_table_view;

    void ReadMemory(uint32_t address);
};
