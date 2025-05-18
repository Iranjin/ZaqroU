#include "MemoryTableView.h"

#include <imgui.h>

#include <utils/StrUtils.h>
#include <utils/TCPGecko.h>

#include <cstdio>


MemoryTableView::MemoryTableView(std::shared_ptr<TCPGecko> tcp)
    : m_base_address(0),
      m_view_mode(EViewMode::HEX),
      m_view_format("%08X"),
      m_selected_cell({ -1, -1 }),
      m_tcp_gecko(tcp)
{
}

void MemoryTableView::SetMemory(const std::vector<uint8_t> &memory, uint32_t base_address)
{
    m_memory = memory;
    m_base_address = base_address;
}

void MemoryTableView::SetViewMode(EViewMode mode)
{
    m_view_mode = mode;

    switch (mode)
    {
    case EViewMode::FLOAT:
        m_bytes_per_cell = 4;
        m_view_format = "%08X";
        break;
    case EViewMode::UTF16:
        m_bytes_per_cell = 2;
        m_view_format = "%04X";
        break;
    case EViewMode::UTF8:
        m_bytes_per_cell = 1;
        m_view_format = "%02X";
        break;
    default:
        m_bytes_per_cell = 4;
        m_view_format = "%08X";
        break;
    }
}

void MemoryTableView::FormatValue(char *str, size_t size, const char *buf)
{
    switch (m_view_mode)
    {
        case EViewMode::HEX:
        {
            uint32_t val = TCPGecko::read_u32_be(buf);
            snprintf(str, size, "%08X", val);
            break;
        }
        case EViewMode::DEC:
        {
            uint32_t val = *(uint32_t*) buf;
            snprintf(str, size, "%u", val);
            break;
        }
        case EViewMode::FLOAT:
        {
            uint32_t bits = TCPGecko::read_u32_be(buf);
            float f;
            std::memcpy(&f, &bits, sizeof(f));
            snprintf(str, size, "%f", f);
            break;
        }
        case EViewMode::UTF8:
        {
            unsigned char c = buf[0];
            if (c >= 32 && c <= 126)
                snprintf(str, size, "%c", c);
            else if (c == 0)
                snprintf(str, size, "");
            else
                snprintf(str, size, ".");
            break;
        }
        case EViewMode::UTF16:
        {
            uint16_t val = buf[0] << 8 | buf[1];

            if (val >= 32 && val <= 126)
                snprintf(str, size, "%c", val);
            else if (val == 0)
                snprintf(str, size, "");
            else
                snprintf(str, size, "U+%04X", val);
            break;
        }
    }
}

void MemoryTableView::JumpToAddress(uint32_t address)
{
    int offset = address - m_base_address;
    if (offset >= 0 && offset + 3 < static_cast<int>(m_memory.size()))
    {
        int row = (offset / 16) * 16;
        int col = 1 + ((offset % 16) / m_bytes_per_cell);
        m_selected_cell = { row, col };
    }
}

void MemoryTableView::UpdateMemory(uint32_t address, uint32_t value)
{
    int offset = address - m_base_address;
    if (offset >= 0 && offset + m_bytes_per_cell - 1 < static_cast<int>(m_memory.size()))
    {
        for (int i = 0; i < m_bytes_per_cell; ++i)
        {
            int shift = 8 * (m_bytes_per_cell - 1 - i);
            m_memory[offset + i] = (value >> shift) & 0xFF;
        }
    }
}

uint32_t MemoryTableView::GetSelectedAddress() const
{
    if (m_selected_cell.row >= 0 && m_selected_cell.col > 0)
    {
        return m_base_address + m_selected_cell.row + (m_selected_cell.col - 1) * m_bytes_per_cell;
    }
    return 0;
}

uint32_t MemoryTableView::GetSelectedValue() const
{
    uint32_t addr = GetSelectedAddress();
    int index = addr - m_base_address;

    if (index >= 0 && index + m_bytes_per_cell - 1 < static_cast<int>(m_memory.size()))
    {
        uint32_t value = 0;
        for (int i = 0; i < m_bytes_per_cell; ++i)
        {
            value <<= 8;
            value |= m_memory[index + i];
        }
        return value;
    }
    return 0;
}

void MemoryTableView::Draw()
{
    if (m_memory.empty())
        return;

    const int row_size = 16;
    const int num_cells_per_row = row_size / m_bytes_per_cell;
    const int num_columns = 1 + num_cells_per_row;

    if (ImGui::BeginTable("MemoryTable", num_columns, ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn("Address");
        for (int i = 0; i < num_cells_per_row; ++i)
        {
            char header[8];
            snprintf(header, sizeof(header), "+0x%X", i * m_bytes_per_cell);
            ImGui::TableSetupColumn(header);
        }
        ImGui::TableHeadersRow();

        for (int row = 0; row < static_cast<int>(m_memory.size()); row += row_size)
        {
            ImGui::TableNextRow();
            ImGui::PushID(row);

            for (int col = 0; col < num_columns; ++col)
            {
                ImGui::TableSetColumnIndex(col);
                ImGui::PushID(col);

                char buf[32] = {};
                if (col == 0)
                {
                    snprintf(buf, sizeof(buf), "%08X", m_base_address + row);
                    ImGui::TextUnformatted(buf);
                }
                else
                {
                    int i = row + (col - 1) * m_bytes_per_cell;

                    if (i + m_bytes_per_cell - 1 < static_cast<int>(m_memory.size()))
                        FormatValue(buf, sizeof(buf), reinterpret_cast<const char*>(&m_memory[i]));
                    else
                        snprintf(buf, sizeof(buf), "--------");

                    bool isSelected = (m_selected_cell.row == row && m_selected_cell.col == col);
                    if (ImGui::Selectable(buf, isSelected, ImGuiSelectableFlags_None, ImVec2(ImGui::GetColumnWidth(), 0)))
                        m_selected_cell = { row, col };
                }

                ImGui::PopID();
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}
