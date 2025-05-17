#pragma once

#include "../IRaimTab.h"

#include "MemoryTableView.h"

#include <vector>
#include <mutex>
#include <cstdint>


class MemoryEditorTab : public IRaimTab
{
private:
    enum class EViewMode : int
    {
        HEX,
        DEC
    };

    std::vector<uint8_t> mMemory;
    uint32_t mBaseAddress;
    uint32_t mMemSize;

    EViewMode mCurrentViewMode = EViewMode::HEX;
    
    bool mAutoRefreshEnabled = false;
    float mRefreshTimer = 0.0f;

    std::string mViewFormat = "%08X";

    char mAddressInput[9] = {};
    char mValueInput[9] = {};

public:
    MemoryEditorTab(RaimUI *raimUI);
    ~MemoryEditorTab();

    void Update() override;

private:
    MemoryTableView mTableView;

    void ReadMemory(uint32_t address);
};
