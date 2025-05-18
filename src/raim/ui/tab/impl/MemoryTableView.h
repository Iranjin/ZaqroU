// MemoryTableView.h

#pragma once

#include <memory>
#include <vector>
#include <cstdint>

#include <utils/tcp_gecko/TCPGecko.h>


class MemoryTableView
{
public:
    enum class EViewMode : int
    {
        HEX,
        DEC,
        FLOAT,
        UTF8,
        UTF16
    };

public:
    MemoryTableView(std::shared_ptr<TCPGecko> tcp);

    void SetMemory(const std::vector<uint8_t> &memory, uint32_t baseAddress);
    void SetViewMode(EViewMode hexMode);
    void Draw();

    void JumpToAddress(uint32_t address);
    void UpdateMemory(uint32_t address, uint32_t value);

    uint32_t GetSelectedAddress() const;
    uint32_t GetSelectedValue() const;

    uint8_t GetBytesPerCell() const { return m_bytes_per_cell; }
    const char *GetViewFormat() const { return m_view_format.c_str(); }

private:
    void FormatValue(char *str, size_t size, const char *buf);

    std::vector<uint8_t> m_memory;
    uint32_t m_base_address;
    EViewMode m_view_mode;
    uint8_t m_bytes_per_cell = 4;
    std::shared_ptr<TCPGecko> m_tcp_gecko;
    std::string m_view_format;

    struct CellCoord
    {
        int row;
        int col;
    };
    CellCoord m_selected_cell;
};
