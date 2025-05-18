#include "byteorder.h"

#include <stdexcept>


void write_u32(char *buf, uint32_t val)
{
    buf[0] = (val >> 24) & 0xFF;
    buf[1] = (val >> 16) & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
}

void write_u16(char *buf, uint16_t val)
{
    buf[0] = (val >> 8) & 0xFF;
    buf[1] = val & 0xFF;
}

uint32_t read_u32_be(const char *buf)
{
    return ((uint32_t) (uint8_t) buf[0] << 24) |
           ((uint32_t) (uint8_t) buf[1] << 16) |
           ((uint32_t) (uint8_t) buf[2] << 8)  |
           ((uint32_t) (uint8_t) buf[3]);
}

uint64_t read_u64_be(const char *buf)
{
    return ((uint64_t) (uint8_t) buf[0] << 56) |
           ((uint64_t) (uint8_t) buf[1] << 48) |
           ((uint64_t) (uint8_t) buf[2] << 40) |
           ((uint64_t) (uint8_t) buf[3] << 32) |
           ((uint64_t) (uint8_t) buf[4] << 24) |
           ((uint64_t) (uint8_t) buf[5] << 16) |
           ((uint64_t) (uint8_t) buf[6] << 8)  |
           ((uint64_t) (uint8_t) buf[7]);
}

uint32_t read_u32_be(const std::vector<uint8_t> &data)
{
    if (data.size() < 4)
        throw std::runtime_error("Data size too small to read uint32_t");

    return ((uint32_t) (uint8_t) data[0] << 24) |
           ((uint32_t) (uint8_t) data[1] << 16) |
           ((uint32_t) (uint8_t) data[2] << 8)  |
           ((uint32_t) (uint8_t) data[3]);
}

uint64_t read_u64_be(const std::vector<uint8_t> &data)
{
    if (data.size() < 8)
        throw std::runtime_error("Data size too small to read uint64_t");

    return ((uint64_t) (uint8_t) data[0] << 56) |
           ((uint64_t) (uint8_t) data[1] << 48) |
           ((uint64_t) (uint8_t) data[2] << 40) |
           ((uint64_t) (uint8_t) data[3] << 32) |
           ((uint64_t) (uint8_t) data[4] << 24) |
           ((uint64_t) (uint8_t) data[5] << 16) |
           ((uint64_t) (uint8_t) data[6] << 8)  |
           ((uint64_t) (uint8_t) data[7]);
}
