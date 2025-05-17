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
      mMemSize(16 * 800)
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
    float deltaTime = ImGui::GetIO().DeltaTime;

    if (mAutoRefreshEnabled)
    {
        mRefreshTimer += deltaTime;
        if (mRefreshTimer >= 0.1f)
        {
            ReadMemory(mBaseAddress);
            mTableView.SetMemory(mMemory, mBaseAddress);
            mRefreshTimer = 0.0f;
        }
    }
    
    std::shared_ptr<TCPGecko> tcp = getRaim()->getTCPGecko();

    ImGui::BeginDisabled(!tcp->is_connected());
    
    if (mMemory.empty())
    {
        mMemory.resize(mMemSize, 0x00);
        mTableView.SetMemory(mMemory, mBaseAddress);
        mTableView.JumpToAddress(mBaseAddress);
    }

    ImGui::Columns(2, "MemoryControlAndTables", false);
    ImGui::SetColumnWidth(0, 200);
    ImGui::Dummy(ImVec2(0, 50));

    { // 左カラム：値入力
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal;

        ImGui::Text("Value");
        if (ImGui::InputText("##Value", mValueInput, sizeof(mValueInput), flags))
        {
            uint32_t value = std::strtoul(mValueInput, nullptr, 16);
            uint32_t addr = std::strtoul(mAddressInput, nullptr, 16);
            tcp->write_mem_32(addr, value);

            mTableView.UpdateMemory(addr, value);
            uint32_t updated = mTableView.GetSelectedValue();
            snprintf(mValueInput, sizeof(mValueInput), mViewFormat.c_str(), updated);
        }
    }

    ImGui::NextColumn();

    { // 右カラム：アドレス・ビュー切替＋描画
        if (ImGui::BeginTable("MemoryEditorControlTable", 3, ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputTextWithHint("##Address", "Address...", mAddressInput, sizeof(mAddressInput),
                                        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal))
            {
                uint32_t addr = std::strtoul(mAddressInput, nullptr, 16);
                if (TCPGecko::valid_range(addr, mMemSize))
                {
                    ReadMemory(addr);
                    mTableView.SetMemory(mMemory, addr);
                    mTableView.JumpToAddress(addr);

                    snprintf(mValueInput, sizeof(mValueInput), mViewFormat.c_str(), mTableView.GetSelectedValue());

                    getConfig()->set("memedit_base_addr", addr);
                    getConfig()->save();
                }
                else
                {
                    snprintf(mAddressInput, sizeof(mAddressInput), "%08X", mBaseAddress);
                }
            }

            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::Checkbox("Auto Refresh", &mAutoRefreshEnabled);

            ImGui::TableNextColumn();
            ImGui::Text("View Mode:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::Combo("##View Mode", (int *)(&mCurrentViewMode), "Hexadecimal\0Decimal\0\0"))
            {
                mTableView.SetViewMode(mCurrentViewMode == EViewMode::HEX);
            }

            ImGui::EndTable();
        }

        ImGui::BeginChild("MemoryEditorScrollable", ImVec2(0, 0), ImGuiChildFlags_None);
        mTableView.Draw();

        uint32_t selectedAddr = mTableView.GetSelectedAddress();
        uint32_t selectedVal = mTableView.GetSelectedValue();
        if (selectedAddr != 0)
        {
            snprintf(mAddressInput, sizeof(mAddressInput), "%08X", selectedAddr);
            snprintf(mValueInput, sizeof(mValueInput), mViewFormat.c_str(), selectedVal);
        }

        ImGui::EndChild();
    }

    ImGui::Columns(1);
    ImGui::EndDisabled();
}
