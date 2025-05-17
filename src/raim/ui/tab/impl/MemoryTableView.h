// MemoryTableView.h

#pragma once

#include <vector>
#include <cstdint>


class MemoryTableView
{
public:
    MemoryTableView();

    void SetMemory(const std::vector<uint8_t> &memory, uint32_t baseAddress);
    void SetViewMode(bool hexMode);
    void Draw();

    void JumpToAddress(uint32_t address);
    void UpdateMemory(uint32_t address, uint32_t value);

    uint32_t GetSelectedAddress() const;
    uint32_t GetSelectedValue() const;

private:
    std::vector<uint8_t> mMemory;
    uint32_t mBaseAddress;
    bool mHexMode;

    struct CellCoord
    {
        int row;
        int col;
    };
    CellCoord mSelectedCell;
};
