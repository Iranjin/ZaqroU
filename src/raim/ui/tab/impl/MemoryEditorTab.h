#pragma once

#include "../IRaimTab.h"

#include <vector>
#include <mutex>
#include <cstdint>


class MemoryEditorTab : public IRaimTab
{
private:
    struct CellCoord
    {
        int row = -1;
        int col = -1;
    };

    enum class EViewMode : int
    {
        HEX,
        DEC
    };

    std::vector<uint8_t> mMemory;
    uint32_t mBaseAddress;
    uint32_t mMemSize;

    CellCoord mSelectedCell;

    EViewMode mCurrentViewMode = EViewMode::HEX;

    std::string mViewFormat = "%08X";

    char mAddressInput[9] = {};
    char mValueInput[9] = {};

public:
    MemoryEditorTab(RaimUI *raimUI);
    ~MemoryEditorTab();

    void Update() override;

private:
    void ReadMemory(uint32_t address);
};
