#pragma once

#include <vector>
#include <cstdint>
#include <cstring>
#include <string>

#include <boost/asio.hpp>

#include "commands.h"


#define CODE_HANDLER_ENABLED_ADDRESS 0x10014CFC
#define CODE_LIST_START_ADDRESS 0x01133000


class TCPGecko
{
private:
    boost::asio::io_context m_io_context;
    boost::asio::ip::tcp::socket m_socket;
    std::string m_ip_address;
    std::recursive_mutex m_mutex;
    
public:
    TCPGecko();
    ~TCPGecko();

    void connect(const std::string &ip_address, uint16_t port = 7331);
    void disconnect();
    
    bool is_connected() const { return m_socket.is_open(); }

    std::string get_ip_address() const;

    std::vector<uint8_t> read_memory(uint32_t address, uint32_t length = 0x4);
    void write_mem_32(uint32_t address, uint32_t value);
    void write_mem_16(uint32_t address, uint16_t value);
    void write_mem_8(uint32_t address, uint8_t value);
    void write_float(uint32_t address, float value);
    void write_double(uint32_t address, double value);
    void upload_memory(uint32_t address, const std::vector<uint8_t> &data);
    void clear_memory(uint32_t address, uint32_t length);
    void write_str(uint32_t address, const std::string &str, bool null_terminated = true);
    void write_wstr(uint32_t address, const std::wstring &wstr, bool null_terminated = true);
    uint32_t follow_pointer(uint32_t base_address, const std::vector<int32_t> &offsets, bool use_memory_read = true);

    void clear_code_list();
    void upload_code_list(const std::vector<uint8_t> &data);
    void enable_code_handler(bool enabled);
    bool is_code_handler_enabled();

    bool check_server_status();
    size_t get_data_buffer_size();

    uint32_t get_symbol(const std::string &rplname, const std::string &symname, uint8_t data_flag = 0);
    uint64_t call(uint32_t address, const std::vector<uint32_t> &args = {}, int recv_size = 4);

    uint32_t malloc(uint32_t size, uint32_t alignment = 4);
    void free(uint32_t address);

    void set_game_mode_description(const std::wstring &description);
    void shutdown();
    void reboot();
    void launch_title(uint64_t title_id);
    uint64_t get_title_id();
    uint32_t get_principal_id();
    std::string get_account_id();
    std::wstring get_mii_name();
    std::string get_server_version();
    uint32_t get_persistent_id();
    uint32_t get_os_version();
    uint32_t get_version_hash();
    uint32_t get_code_handler_address();

    static bool valid_range(uint32_t address, uint32_t length);
    static bool valid_access(uint32_t address, uint32_t length, const std::string &access);
};
