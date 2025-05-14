#include "MemoryEditorTab.h"

#include <iostream>
#include <utils/TCPGecko.h>
#include <utils/Config.h>
#include <raim/ui/RaimUI.h>
#include <raim/Raim.h>

#include <imgui.h>
#include <cstdlib>


MemoryEditorTab::MemoryEditorTab(RaimUI *raimUI)
    : IRaimTab(raimUI, "MemoryEditor"),
      mMemSize(16 * 200)
{
    mBaseAddress = getConfig()->get("memedit_base_addr", 0x40000000);
    snprintf(mAddressInput, sizeof(mAddressInput), "%08X", mBaseAddress);
}

MemoryEditorTab::~MemoryEditorTab()
{
}

void MemoryEditorTab::ReadMemory(uint32_t address)
{
    try
    {
        std::shared_ptr<TCPGecko> tcp = getRaim()->getTCPGecko();
        if (tcp && tcp->is_connected())
        {
            mMemory = tcp->read_memory(address, mMemSize);
            mBaseAddress = address;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Memory read error: " << e.what() << std::endl;
    }
}

void MemoryEditorTab::Update()
{
    std::shared_ptr<TCPGecko> tcp = getRaim()->getTCPGecko();

    ImGui::BeginDisabled(!tcp->is_connected());

    ImGui::Columns(2, "MemoryControlAndTables", false);
    ImGui::SetColumnWidth(0, 200);
    ImGui::Dummy(ImVec2(0, 50));

    { // 左カラム：アドレス・値入力
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal;

        ImGui::Text("Value");
        if (ImGui::InputText("##Value", mValueInput, sizeof(mValueInput), flags))
        {
            uint32_t value = std::strtoul(mValueInput, nullptr, 16);
            uint32_t addr = std::strtoul(mAddressInput, nullptr, 16);
            tcp->write_memory(addr, value);
            ReadMemory(mBaseAddress);
        }
    }

    ImGui::NextColumn();

    { // 右カラム：表示モード切替とテーブル
    
        if (ImGui::InputTextWithHint("Address", "Address...", mAddressInput, sizeof(mAddressInput),
                                     ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal))
        {
            uint32_t addr = std::strtoul(mAddressInput, nullptr, 16);
            ReadMemory(addr);

            int index = addr - mBaseAddress;
            uint32_t val = (mMemory[index] << 24) | (mMemory[index + 1] << 16) |
                           (mMemory[index + 2] << 8) | mMemory[index + 3];
            snprintf(mValueInput, sizeof(mValueInput), mViewFormat.c_str(), val);

            getConfig()->set("memedit_base_addr", addr);
            getConfig()->save();
        }
        ImGui::SameLine();
        const char *items[] = { "Hexadecimal", "Decimal" };
        ImGui::Text("View Mode:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::Combo("##View Mode", (int*) (&mCurrentViewMode), "Hexadecimal\0Decimal\0\0");

        ImGui::BeginChild("MemoryEditorScrollable", ImVec2(0, 0), ImGuiChildFlags_None);
        
        std::vector<uint8_t> displayMemory = mMemory;
        if (displayMemory.empty())
            displayMemory.resize(mMemSize, 0x00); // ゼロ埋め

        if (ImGui::BeginTable("MemoryTable", 5, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Address");
            ImGui::TableSetupColumn("+0x0");
            ImGui::TableSetupColumn("+0x4");
            ImGui::TableSetupColumn("+0x8");
            ImGui::TableSetupColumn("+0xC");
            ImGui::TableHeadersRow();

            for (int row = 0; row < displayMemory.size(); row += 16)
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
                    }
                    else
                    {
                        int i = row + (col - 1) * 4;
                        if (i + 3 < displayMemory.size())
                        {
                            uint32_t val = (displayMemory[i] << 24) | (displayMemory[i + 1] << 16) |
                                        (displayMemory[i + 2] << 8) | displayMemory[i + 3];
                            if (mCurrentViewMode == EViewMode::HEX)
                                snprintf(buf, sizeof(buf), "%08X", val);
                            else
                                snprintf(buf, sizeof(buf), "%u", val);
                        }
                        else
                        {
                            snprintf(buf, sizeof(buf), "--------");
                        }
                    }

                    bool isSelected = (mSelectedCell.row == row && mSelectedCell.col == col);

                    ImGuiSelectableFlags flags = (col == 0) ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None;
                    if (ImGui::Selectable(buf, isSelected, flags, ImVec2(ImGui::GetColumnWidth(), 0)))
                    {
                        mSelectedCell = { row, col };

                        uint32_t selectedAddress = mBaseAddress + row + (col - 1) * 4;
                        snprintf(mAddressInput, sizeof(mAddressInput), "%08X", selectedAddress);

                        if (selectedAddress + 3 < mBaseAddress + displayMemory.size())
                        {
                            int index = selectedAddress - mBaseAddress;
                            uint32_t val = (displayMemory[index] << 24) | (displayMemory[index + 1] << 16) |
                                        (displayMemory[index + 2] << 8) | displayMemory[index + 3];
                            snprintf(mValueInput, sizeof(mValueInput), "%08X", val);
                        }
                    }

                    ImGui::PopID();
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        ImGui::EndChild();
    }

    ImGui::Columns(1);
    ImGui::EndDisabled();
}
