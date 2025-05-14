#include "TCPGecko.h"

#include <iostream>
#include <stdexcept>


TCPGecko::TCPGecko()
    : m_socket(m_io_context), m_connected(false)
{
}

TCPGecko::~TCPGecko()
{
    if (is_connected())
        disconnect();
}

void TCPGecko::connect(const std::string &ip_address, uint16_t port)
{
    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::make_address(ip_address), port);
    m_socket.connect(endpoint);
    m_connected = true;
}

void TCPGecko::disconnect()
{
    m_socket.close();
    m_connected = false;
}

std::vector<uint8_t> TCPGecko::read_memory(uint32_t address, uint32_t length)
{
    if (!is_connected())
        throw std::runtime_error("Not connected");
    
    char command = (char) COMMAND_READ_MEMORY;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char request[8];
    write_u32(request, address);
    write_u32(request + 4, address + length);

    boost::asio::write(m_socket, boost::asio::buffer(request, 8));

    char status;
    boost::asio::read(m_socket, boost::asio::buffer(&status, 1));

    std::vector<uint8_t> result(length);

    if (status == '\xbd')
        boost::asio::read(m_socket, boost::asio::buffer(result.data(), length));
    // else if (status == '\xb0')
    //     std::fill(result.begin(), result.end(), 0);
    else
        std::fill(result.begin(), result.end(), 0);

    return result;
}

void TCPGecko::write_memory(uint32_t address, uint32_t value)
{
    if (!is_connected())
        throw std::runtime_error("Not connected");
    
    char command = (char) COMMAND_WRITE_32;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char request[8];
    write_u32(request, address);
    write_u32(request + 4, value);

    boost::asio::write(m_socket, boost::asio::buffer(request, 8));
}

void TCPGecko::write_memory(uint32_t address, uint16_t value)
{
    if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_WRITE_16;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char request[8] = {0};
    write_u32(request, address);
    write_u16(request + 6, value);

    boost::asio::write(m_socket, boost::asio::buffer(request, 8));
}

void TCPGecko::write_memory(uint32_t address, uint8_t value)
{
    if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_WRITE_8;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char request[8] = {0};
    write_u32(request, address);
    request[7] = value;

    boost::asio::write(m_socket, boost::asio::buffer(request, 8));
}

void TCPGecko::upload_memory(uint32_t address, const std::vector<uint8_t> &data)
{
    if (!is_connected())
        throw std::runtime_error("Not connected");
    
    if (data.empty())
        return;

    size_t max_size = get_data_buffer_size();
    if (data.size() > max_size)
        throw std::runtime_error("Data size exceeds maximum buffer size");
    
    char command = (char) COMMAND_UPLOAD_MEMORY;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char request[8];
    write_u32(request, address);
    write_u32(request + 4, address + data.size());

    boost::asio::write(m_socket, boost::asio::buffer(request, 8));
    boost::asio::write(m_socket, boost::asio::buffer(data.data(), data.size()));
}

void TCPGecko::clear_memory(uint32_t address, uint32_t length)
{
    if (!is_connected())
        throw std::runtime_error("Not connected");

    if (length == 0)
        return;

    size_t max_size = get_data_buffer_size();
    uint32_t remaining = length;

    while (remaining > 0)
    {
        uint32_t chunk_size = std::min<uint32_t>(remaining, max_size);
        std::vector<uint8_t> zero_data(chunk_size, 0x00);
        upload_memory(address, zero_data);
        address += chunk_size;
        remaining -= chunk_size;
    }
}

void TCPGecko::upload_code_list(std::vector<uint8_t> data)
{
    for (size_t i = 0; i < 4; i++)
        data.push_back(0x00);
    upload_memory(CODE_LIST_START_ADDRESS, data);
}

void TCPGecko::enable_code_handler(bool enabled)
{
    write_memory(CODE_HANDLER_ENABLED_ADDRESS, (uint32_t) enabled);
}

bool TCPGecko::is_code_handler_enabled()
{
    return (bool) read_memory(CODE_HANDLER_ENABLED_ADDRESS)[3];
}

size_t TCPGecko::get_data_buffer_size()
{
    if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_GET_DATA_BUFFER_SIZE;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char response[4];
    boost::asio::read(m_socket, boost::asio::buffer(response, 4));
    size_t size = read_u32_be(response);

    return size;
}

uint32_t TCPGecko::get_symbol(const std::string &rplname, const std::string &symname, uint8_t data_flag)
{
    if (!is_connected())
        throw std::runtime_error("Not connected");
    
    char cmd = (char) COMMAND_GET_SYMBOL;
    boost::asio::write(m_socket, boost::asio::buffer(&cmd, 1));

    std::vector<char> request;

    uint32_t rplname_offset = 8;
    uint32_t symname_offset = 8 + rplname.size() + 1;

    request.push_back((rplname_offset >> 24) & 0xFF);
    request.push_back((rplname_offset >> 16) & 0xFF);
    request.push_back((rplname_offset >> 8) & 0xFF);
    request.push_back((rplname_offset >> 0) & 0xFF);

    request.push_back((symname_offset >> 24) & 0xFF);
    request.push_back((symname_offset >> 16) & 0xFF);
    request.push_back((symname_offset >> 8) & 0xFF);
    request.push_back((symname_offset >> 0) & 0xFF);

    request.insert(request.end(), rplname.begin(), rplname.end());
    request.push_back('\0');
    request.insert(request.end(), symname.begin(), symname.end());
    request.push_back('\0');

    uint8_t length = request.size();

    boost::asio::write(m_socket, boost::asio::buffer(&length, 1));
    boost::asio::write(m_socket, boost::asio::buffer(request));
    boost::asio::write(m_socket, boost::asio::buffer(&data_flag, 1));

    char address_buf[4];
    boost::asio::read(m_socket, boost::asio::buffer(address_buf, 4));
    uint32_t address = read_u32_be(address_buf);

    return address;
}

uint64_t TCPGecko::call(uint32_t address, const std::vector<uint32_t> &args, int recv_size)
{
    if (!is_connected())
        throw std::runtime_error("Not connected");

    if (args.size() > 8)
        throw std::invalid_argument("Too many arguments (max 8)");

    std::vector<uint32_t> padded_args = args;
    padded_args.resize(8, 0);

    char cmd = (char) COMMAND_REMOTE_PROCEDURE_CALL;
    boost::asio::write(m_socket, boost::asio::buffer(&cmd, 1));

    char request[36];
    write_u32(request, address);

    for (size_t i = 0; i < 8; ++i)
        write_u32(request + 4 + i * 4, padded_args[i]);

    boost::asio::write(m_socket, boost::asio::buffer(request, sizeof(request)));

    char result_buf[8];
    boost::asio::read(m_socket, boost::asio::buffer(result_buf, 8));
    uint64_t result = read_u64_be(result_buf);

    if (recv_size == 4)
        return (uint32_t) (result >> 32);
    else
        return result;
}

uint64_t TCPGecko::get_title_id()
{
    return call(get_symbol("coreinit.rpl", "OSGetTitleID"), {}, 8);
}

std::string TCPGecko::get_server_version()
{
    if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_SERVER_VERSION;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char length_buf[4];
    boost::asio::read(m_socket, boost::asio::buffer(length_buf, 4));
    int version_length = read_u32_be(length_buf);

    std::vector<char> version_buf(version_length);
    boost::asio::read(m_socket, boost::asio::buffer(version_buf.data(), version_length));

    return std::string(version_buf.begin(), version_buf.end());
}

uint32_t TCPGecko::get_os_version()
{
    if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_GET_OS_VERSION;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char response[4];
    boost::asio::read(m_socket, boost::asio::buffer(response, 4));
    return read_u32_be(response);
}

uint32_t TCPGecko::get_version_hash()
{
    if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_GET_VERSION_HASH;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char response[4];
    boost::asio::read(m_socket, boost::asio::buffer(response, 4));
    return read_u32_be(response);
}

// TODO: 未実装
std::string TCPGecko::get_account_id()
{
    /*if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_ACCOUNT_IDENTIFIER;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char length_buf[4];
    boost::asio::read(m_socket, boost::asio::buffer(length_buf, 4));
    int version_length = read_u32_be(length_buf);

    std::vector<char> version_buf(version_length);
    boost::asio::read(m_socket, boost::asio::buffer(version_buf.data(), version_length));

    return std::string(version_buf.begin(), version_buf.end());*/
    return "";
}

uint32_t TCPGecko::get_code_handler_address()
{
    if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_GET_CODE_HANDLER_ADDRESS;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char response[4];
    boost::asio::read(m_socket, boost::asio::buffer(response, 4));
    return read_u32_be(response);
}

bool TCPGecko::valid_range(uint32_t address, uint32_t length)
{
    if (address + length < address)
        return false;

    if (0x01000000 <= address && address + length <= 0x01800000) return true;
    else if (0x0E000000 <= address && address + length <= 0x10000000) return true;  // Depends on game
    else if (0x10000000 <= address && address + length <= 0x50000000) return true;  // Doesn't quite go to 5
    else if (0xE0000000 <= address && address + length <= 0xE4000000) return true;
    else if (0xE8000000 <= address && address + length <= 0xEA000000) return true;
    else if (0xF4000000 <= address && address + length <= 0xF6000000) return true;
    else if (0xF6000000 <= address && address + length <= 0xF6800000) return true;
    else if (0xF8000000 <= address && address + length <= 0xFB000000) return true;
    else if (0xFB000000 <= address && address + length <= 0xFB800000) return true;
    else if (0xFFFE0000 <= address && address + length <= 0xFFFFFFFF) return true;
    else return false;
}

bool TCPGecko::valid_access(uint32_t address, uint32_t length, const std::string &access)
{
    if (address + length < address)
        return false;

    if (0x01000000 <= address && address + length <= 0x01800000)
    {
        if (access == "read") return true;
        else if (access == "write") return false;
    }
    else if (0x0E000000 <= address && address + length <= 0x10000000)
    {  // Depends on game
        if (access == "read") return true;
        else if (access == "write") return false;
    }
    else if (0x10000000 <= address && address + length <= 0x50000000)
    {
        if (access == "read" || access == "write") return true;
    }
    else if (0xE0000000 <= address && address + length <= 0xE4000000)
    {
        if (access == "read") return true;
        else if (access == "write") return false;
    }
    else if (0xE8000000 <= address && address + length <= 0xEA000000)
    {
        if (access == "read") return true;
        else if (access == "write") return false;
    }
    else if (0xF4000000 <= address && address + length <= 0xF6000000)
    {
        if (access == "read") return true;
        else if (access == "write") return false;
    }
    else if (0xF6000000 <= address && address + length <= 0xF6800000)
    {
        if (access == "read") return true;
        else if (access == "write") return false;
    }
    else if (0xF8000000 <= address && address + length <= 0xFB000000)
    {
        if (access == "read") return true;
        else if (access == "write") return false;
    }
    else if (0xFB000000 <= address && address + length <= 0xFB800000)
    {
        if (access == "read") return true;
        else if (access == "write") return false;
    }
    else if (0xFFFE0000 <= address && address + length <= 0xFFFFFFFF)
    {
        if (access == "read" || access == "write") return true;
    }

    return false;
}

void TCPGecko::write_u32(char *buf, uint32_t val)
{
    buf[0] = (val >> 24) & 0xFF;
    buf[1] = (val >> 16) & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
}

void TCPGecko::write_u16(char *buf, uint16_t val)
{
    buf[0] = (val >> 8) & 0xFF;
    buf[1] = val & 0xFF;
}

uint32_t TCPGecko::read_u32_be(const char *buf)
{
    return ((uint32_t) (uint8_t) buf[0] << 24) |
           ((uint32_t) (uint8_t) buf[1] << 16) |
           ((uint32_t) (uint8_t) buf[2] << 8)  |
           ((uint32_t) (uint8_t) buf[3]);
}

uint64_t TCPGecko::read_u64_be(const char *buf)
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

