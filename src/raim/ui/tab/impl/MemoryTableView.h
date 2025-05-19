// MemoryTableView.h

#pragma once

#include <memory>
#include <vector>
#include <cstdint>


class TCPGecko;

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

    void set_memory(const std::vector<uint8_t> &memory, uint32_t base_address);
    void set_view_mode(EViewMode hexMode);
    void draw();

    void jump_to_address(uint32_t address);
    void update_memory(uint32_t address, uint32_t value);

    uint32_t get_selected_address() const;
    uint32_t get_selected_value() const;

    uint8_t get_bytes_per_cell() const { return m_bytes_per_cell; }
    const char *get_view_format() const { return m_view_format.c_str(); }

private:
    void format_value(char *str, size_t size, const char *buf);

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
