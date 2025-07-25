#include "MemoryEditorTab.h"

#include <iostream>
#include <utils/tcp_gecko/TCPGecko.h>
#include <utils/Config.h>
#include <utils/common.h>
#include <raim/ui/RaimUI.h>
#include <raim/Raim.h>

#include <imgui.h>
#include <cstdlib>


MemoryEditorTab::MemoryEditorTab(RaimUI *raim_ui)
    : IRaimTab(raim_ui, "MemoryEditor"),
      m_mem_size(16 * 800),
      m_table_view(std::make_shared<MemoryTableView>(get_raim()->get_tcp_gecko()))
{
    m_base_address = get_config()->get_nested("memory_editor.base_address", 0x40000000);
    snprintf(m_address_input, sizeof(m_address_input), "%08X", m_base_address);

    m_auto_refresh_enabled = get_config()->get_nested("memory_editor.auto_refresh", false);
}

MemoryEditorTab::~MemoryEditorTab()
{
}

void MemoryEditorTab::ReadMemory(uint32_t address)
{
    try
    {
        std::shared_ptr<TCPGecko> tcp = get_raim()->get_tcp_gecko();
        if (tcp && tcp->is_connected())
        {
            m_memory = tcp->read_memory(address, m_mem_size);
            m_base_address = address;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Memory read error: " << e.what() << std::endl;
    }
}

void MemoryEditorTab::Update()
{
    float delta_time = ImGui::GetIO().DeltaTime;

    if (m_auto_refresh_enabled)
    {
        m_refresh_timer += delta_time;
        if (m_refresh_timer >= 0.1f)
        {
            ReadMemory(m_base_address);
            m_table_view->set_memory(m_memory, m_base_address);
            m_refresh_timer = 0.0f;
        }
    }
    
    std::shared_ptr<TCPGecko> tcp = get_raim()->get_tcp_gecko();

    ImGui::BeginDisabled(!tcp->is_connected());
    
    if (m_memory.empty())
    {
        m_memory.resize(m_mem_size, 0x00);
        m_table_view->set_memory(m_memory, m_base_address);
        m_table_view->jump_to_address(m_base_address);
    }

    ImGui::Columns(2, "MemoryControlAndTables", false);
    ImGui::SetColumnWidth(0, 200);
    ImGui::Dummy(ImVec2(0, 50));

    { // 左カラム：値入力
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal;

        ImGui::Text("Value");
        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::InputText("##Value", m_value_input, sizeof(m_value_input), flags))
        {
            uint32_t addr = std::strtoul(m_address_input, nullptr, 16);
            uint32_t value = std::strtoul(m_value_input, nullptr, 16);

            switch (m_table_view->get_bytes_per_cell())
            {
                case 4: tcp->write_mem_32(addr, value); break;
                case 2: tcp->write_mem_16(addr, value); break;
                case 1: tcp->write_mem_8(addr, value); break;
            }
            
            m_table_view->update_memory(addr, value);
            uint32_t updated = m_table_view->get_selected_value();
            snprintf(m_value_input, sizeof(m_value_input), m_table_view->get_view_format(), updated);
        }
    }

    ImGui::NextColumn();

    { // 右カラム：アドレス・ビュー切替＋描画
        if (ImGui::BeginTable("MemoryEditorControlTable", 3, ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputTextWithHint("##Address", "Address...", m_address_input, sizeof(m_address_input),
                                         ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsHexadecimal))
            {
                uint32_t addr = std::strtoul(m_address_input, nullptr, 16);
                if (TCPGecko::valid_range(addr, m_mem_size))
                {
                    ReadMemory(addr);
                    m_table_view->set_memory(m_memory, addr);
                    m_table_view->jump_to_address(addr);

                    snprintf(m_value_input, sizeof(m_value_input), "%08X", m_table_view->get_selected_value());

                    get_config()->set_nested("memory_editor.base_address", addr);
                    get_config()->save();
                }
                else
                {
                    snprintf(m_address_input, sizeof(m_address_input), "%08X", m_base_address);
                }
            }

            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::Checkbox("Auto Refresh", &m_auto_refresh_enabled))
            {
                get_config()->set_nested("memory_editor.auto_refresh", m_auto_refresh_enabled);
                get_config()->save();
            }

            ImGui::TableNextColumn();
            ImGui::Text("View Mode:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::Combo("##View Mode", (int *) (&m_view_mode), "Hexadecimal\0Decimal\0Float\0UTF-8\0UTF-16\0"))
                m_table_view->set_view_mode(m_view_mode);

            ImGui::EndTable();
        }

        ImGui::BeginChild("MemoryEditorScrollable", ImVec2(0, 0), ImGuiChildFlags_None);
        m_table_view->draw();

        uint32_t selected_addr = m_table_view->get_selected_address();
        uint32_t selected_val = m_table_view->get_selected_value();
        if (selected_addr != 0)
        {
            snprintf(m_address_input, sizeof(m_address_input), "%08X", selected_addr);
            snprintf(m_value_input, sizeof(m_value_input), m_table_view->get_view_format(), selected_val);
        }

        ImGui::EndChild();
    }

    ImGui::Columns(1);
    ImGui::EndDisabled();
}

void MemoryEditorTab::OnConnected()
{
    if (get_raim()->get_tcp_gecko()->is_connected())
    {
        ReadMemory(m_base_address);
        m_table_view->set_memory(m_memory, m_base_address);
    }
}

void MemoryEditorTab::OnTabOpened()
{
    if (get_raim()->get_tcp_gecko()->is_connected())
    {
        ReadMemory(m_base_address);
        m_table_view->set_memory(m_memory, m_base_address);
    }
}
