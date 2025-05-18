#pragma once

#include <cstdint>
#include <vector>


void write_u32(char *buf, uint32_t val);
void write_u16(char *buf, uint16_t val);
uint32_t read_u32_be(const char *buf);
uint64_t read_u64_be(const char *buf);
uint32_t read_u32_be(const std::vector<uint8_t> &data);
uint64_t read_u64_be(const std::vector<uint8_t> &data);
