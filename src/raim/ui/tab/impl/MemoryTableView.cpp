#include "MemoryTableView.h"

#include <imgui.h>
#include <cstdio>


MemoryTableView::MemoryTableView()
    : mBaseAddress(0),
      mHexMode(true),
      mSelectedCell({ -1, -1 })
{
}

void MemoryTableView::SetMemory(const std::vector<uint8_t> &memory, uint32_t baseAddress)
{
    mMemory = memory;
    mBaseAddress = baseAddress;
}

void MemoryTableView::SetViewMode(bool hexMode)
{
    mHexMode = hexMode;
}

void MemoryTableView::JumpToAddress(uint32_t address)
{
    int offset = address - mBaseAddress;
    if (offset >= 0 && offset + 3 < static_cast<int>(mMemory.size()))
    {
        int row = (offset / 16) * 16;
        int col = 1 + ((offset % 16) / 4);
        mSelectedCell = { row, col };
    }
}

void MemoryTableView::UpdateMemory(uint32_t address, uint32_t value)
{
    int offset = address - mBaseAddress;
    if (offset >= 0 && offset + 3 < static_cast<int>(mMemory.size()))
    {
        mMemory[offset]     = (value >> 24) & 0xFF;
        mMemory[offset + 1] = (value >> 16) & 0xFF;
        mMemory[offset + 2] = (value >> 8) & 0xFF;
        mMemory[offset + 3] = value & 0xFF;
    }
}

uint32_t MemoryTableView::GetSelectedAddress() const
{
    if (mSelectedCell.row >= 0 && mSelectedCell.col > 0)
    {
        return mBaseAddress + mSelectedCell.row + (mSelectedCell.col - 1) * 4;
    }
    return 0;
}

uint32_t MemoryTableView::GetSelectedValue() const
{
    uint32_t addr = GetSelectedAddress();
    int index = addr - mBaseAddress;
    if (index >= 0 && index + 3 < static_cast<int>(mMemory.size()))
    {
        return (mMemory[index] << 24) |
               (mMemory[index + 1] << 16) |
               (mMemory[index + 2] << 8) |
               mMemory[index + 3];
    }
    return 0;
}

void MemoryTableView::Draw()
{
    std::vector<uint8_t> displayMemory = mMemory;
    if (displayMemory.empty())
        return;

    if (ImGui::BeginTable("MemoryTable", 5, ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("+0x0");
        ImGui::TableSetupColumn("+0x4");
        ImGui::TableSetupColumn("+0x8");
        ImGui::TableSetupColumn("+0xC");
        ImGui::TableHeadersRow();

        for (int row = 0; row < static_cast<int>(displayMemory.size()); row += 16)
        {
            ImGui::TableNextRow();
            ImGui::PushID(row);

            for (int col = 0; col < 5; ++col)
            {
                ImGui::TableSetColumnIndex(col);
                ImGui::PushID(col);

                char buf[16] = {};
                if (col == 0)
                {
                    snprintf(buf, sizeof(buf), "%08X", mBaseAddress + row);
                    ImGui::TextUnformatted(buf);
                }
                else
                {
                    int i = row + (col - 1) * 4;
                    if (i + 3 < static_cast<int>(displayMemory.size()))
                    {
                        uint32_t val = (displayMemory[i] << 24) |
                                       (displayMemory[i + 1] << 16) |
                                       (displayMemory[i + 2] << 8) |
                                       (displayMemory[i + 3]);
                        if (mHexMode)
                            snprintf(buf, sizeof(buf), "%08X", val);
                        else
                            snprintf(buf, sizeof(buf), "%u", val);
                    }
                    else
                    {
                        snprintf(buf, sizeof(buf), "--------");
                    }

                    bool isSelected = (mSelectedCell.row == row && mSelectedCell.col == col);

                    ImGuiSelectableFlags flags = ImGuiSelectableFlags_None;
                    if (ImGui::Selectable(buf, isSelected, flags, ImVec2(ImGui::GetColumnWidth(), 0)))
                    {
                        mSelectedCell = { row, col };
                    }
                }

                ImGui::PopID();
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}
