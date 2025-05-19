#include "TCPGecko.h"

#include "byteorder.h"

#include <iostream>
#include <stdexcept>


TCPGecko::TCPGecko()
    : m_socket(m_io_context), m_nagle_enabled(false)
{
}

TCPGecko::~TCPGecko()
{
    if (is_connected())
        disconnect();
}

void TCPGecko::connect(const std::string &ip_address, uint16_t port)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
    try
    {
        boost::asio::ip::tcp::endpoint endpoint(
            boost::asio::ip::make_address(ip_address), port);
        m_socket.connect(endpoint);
        m_ip_address = ip_address;

        set_nagle_enabled(m_nagle_enabled);
    }
    catch (const std::exception &e)
    {
        m_socket.close();
        throw;
    }
}

void TCPGecko::disconnect()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
    m_socket.close();
    m_ip_address.clear();
}

std::vector<uint8_t> TCPGecko::read_memory(uint32_t address, uint32_t length)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

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

void TCPGecko::write_mem_32(uint32_t address, uint32_t value)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    if (!is_connected())
        throw std::runtime_error("Not connected");
    
    char command = (char) COMMAND_WRITE_32;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char request[8];
    write_u32(request, address);
    write_u32(request + 4, value);

    boost::asio::write(m_socket, boost::asio::buffer(request, 8));
}

void TCPGecko::write_mem_16(uint32_t address, uint16_t value)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
    if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_WRITE_16;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char request[8] = {0};
    write_u32(request, address);
    write_u16(request + 6, value);

    boost::asio::write(m_socket, boost::asio::buffer(request, 8));
}

void TCPGecko::write_mem_8(uint32_t address, uint8_t value)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
    if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_WRITE_8;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char request[8] = {0};
    write_u32(request, address);
    request[7] = value;

    boost::asio::write(m_socket, boost::asio::buffer(request, 8));
}

void TCPGecko::write_float(uint32_t address, float value)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
    if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_WRITE_32;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    uint32_t binary_value;
    std::memcpy(&binary_value, &value, sizeof(float));

    char request[8];
    write_u32(request, address);
    write_u32(request + 4, binary_value);

    boost::asio::write(m_socket, boost::asio::buffer(request, 8));
}

void TCPGecko::write_double(uint32_t address, double value)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
    if (!is_connected())
        throw std::runtime_error("Not connected");

    uint64_t binary_value;
    std::memcpy(&binary_value, &value, sizeof(double));

    uint32_t high = (uint32_t)(binary_value >> 32);
    uint32_t low  = (uint32_t)(binary_value & 0xFFFFFFFF);

    {
        char command = (char) COMMAND_WRITE_32;
        boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

        char request[8];
        write_u32(request, address);
        write_u32(request + 4, high);
        boost::asio::write(m_socket, boost::asio::buffer(request, 8));
    }

    {
        char command = (char) COMMAND_WRITE_32;
        boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

        char request[8];
        write_u32(request, address + 4);
        write_u32(request + 4, low);
        boost::asio::write(m_socket, boost::asio::buffer(request, 8));
    }
}

void TCPGecko::upload_memory(uint32_t address, const std::vector<uint8_t> &data)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
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

void TCPGecko::write_str(uint32_t address, const std::string &str, bool null_terminated)
{
    std::vector<uint8_t> data(str.begin(), str.end());
    if (null_terminated)
        data.push_back(0x00);
    upload_memory(address, data);
}

void TCPGecko::write_wstr(uint32_t address, const std::wstring &wstr, bool null_terminated)
{
    std::vector<uint8_t> data;
    for (wchar_t ch : wstr)
    {
        uint16_t val = static_cast<uint16_t>(ch);
        data.push_back((val >> 8) & 0xFF);
        data.push_back(val & 0xFF);
    }

    if (null_terminated)
    {
        data.push_back(0x00);
        data.push_back(0x00);
    }

    upload_memory(address, data);
}

uint32_t TCPGecko::follow_pointer(uint32_t base_address, const std::vector<int32_t> &offsets, bool use_memory_read)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
    if (!is_connected())
        throw std::runtime_error("Not connected");

    if (use_memory_read)
    {
        uint32_t address = base_address;
        for (size_t i = 0; i < offsets.size(); ++i)
        {
            if (!valid_range(address, 4))
                throw std::runtime_error("Invalid memory range during pointer traversal");

            std::vector<uint8_t> data = read_memory(address, 4);
            if (data.size() != 4)
                throw std::runtime_error("Failed to read memory");

            address = read_u32_be(data);
            address += offsets[i];
        }
        return address;
    }
    else
    {
        char command = (char) COMMAND_FOLLOW_POINTER;
        boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

        char request[8];
        write_u32(request, base_address);
        write_u32(request + 4, offsets.size());

        boost::asio::write(m_socket, boost::asio::buffer(request, 8));
        if (!offsets.empty())
            boost::asio::write(m_socket, boost::asio::buffer(offsets.data(), offsets.size() * sizeof(int32_t)));

        char response[4];
        boost::asio::read(m_socket, boost::asio::buffer(response, 4));
        return read_u32_be(response);
    }
}

void TCPGecko::upload_code_list(std::vector<uint8_t> data)
{
    for (size_t i = 0; i < 8; i++)
        data.push_back(0x00);
    upload_memory(CODE_LIST_START_ADDRESS, data);
}

void TCPGecko::enable_code_handler(bool enabled)
{
    write_mem_8(CODE_HANDLER_ENABLED_ADDRESS, enabled);
}

bool TCPGecko::is_code_handler_enabled()
{
    return (bool) read_memory(CODE_HANDLER_ENABLED_ADDRESS)[3];
}

size_t TCPGecko::get_data_buffer_size()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
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
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
    if (!is_connected())
        throw std::runtime_error("Not connected");
    
    char command = (char) COMMAND_GET_SYMBOL;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

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
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
    if (!is_connected())
        throw std::runtime_error("Not connected");

    if (args.size() > 8)
        throw std::invalid_argument("Too many arguments (max 8)");

    std::vector<uint32_t> padded_args = args;
    padded_args.resize(8, 0);

    char command = (char) COMMAND_REMOTE_PROCEDURE_CALL;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    constexpr size_t REQUEST_SIZE = 4 + 8 * 4;
    char request[REQUEST_SIZE];
    write_u32(request, address);

    for (size_t i = 0; i < 8; ++i)
        write_u32(request + 4 + i * 4, padded_args[i]);

    boost::asio::write(m_socket, boost::asio::buffer(request, sizeof(request)));

    char result_buf[8];
    boost::asio::read(m_socket, boost::asio::buffer(result_buf, 8));
    uint64_t result = read_u64_be(result_buf);

    if (recv_size == 4)
        return result >> 32;
    else if (recv_size == 8)
        return result;
    else
        throw std::invalid_argument("recv_size must be either 4 or 8");
}

uint32_t TCPGecko::malloc(uint32_t size, uint32_t alignment)
{
    uint32_t address = call(get_symbol("coreinit.rpl", "OSAllocFromSystem"), {size, alignment});
    if (address == 0x0)
        throw std::runtime_error("Failed to allocate memory");
    return address;
}

void TCPGecko::free(uint32_t address)
{
    call(get_symbol("coreinit.rpl", "OSFreeToSystem"), {address});
}

void TCPGecko::set_game_mode_description(const std::wstring &description)
{
    uint32_t wstr_ptr = malloc(description.size() * sizeof(wchar_t));
    write_wstr(wstr_ptr, description);
    call(get_symbol("nn_fp.rpl", "UpdateGameModeDescription__Q2_2nn2fpFPCw"), {wstr_ptr});
    free(wstr_ptr);
}

void TCPGecko::shutdown()
{
    call(get_symbol("coreinit.rpl", "OSShutdown"));
}

void TCPGecko::reboot()
{
    call(get_symbol("coreinit.rpl", "OSLaunchTitlel"), {0xFFFFFFFF, 0xFFFFFFFE});
}

void TCPGecko::launch_title(uint64_t title_id)
{
    uint32_t upper_id = title_id >> 32;
    uint32_t lower_id = title_id & 0xFFFFFFFF;
    call(get_symbol("sysapp.rpl", "SYSLaunchTitle"), {upper_id, lower_id});
}

uint64_t TCPGecko::get_title_id()
{
    return call(get_symbol("coreinit.rpl", "OSGetTitleID"), {}, 8);
}

uint32_t TCPGecko::get_principal_id()
{
    return call(get_symbol("nn_act.rpl", "GetPrincipalId__Q2_2nn3actFv"));
}

std::string TCPGecko::get_account_id()
{
    if (!is_connected())
        throw std::runtime_error("Not connected");

    constexpr uint32_t AccountIdSize = 16;
    constexpr uint32_t Alignment = 4;

    uint32_t buffer_address = malloc(AccountIdSize, Alignment);

    try
    {
        clear_memory(buffer_address, AccountIdSize);

        uint32_t func_addr = get_symbol("nn_act.rpl", "GetAccountId__Q2_2nn3actFPc");
        call(func_addr, {buffer_address});

        std::vector<uint8_t> account_id_data = read_memory(buffer_address, AccountIdSize);

        size_t length = 0;
        for (; length < AccountIdSize; ++length)
        {
            if (account_id_data[length] == 0)
                break;
        }

        free(buffer_address);
        return std::string(account_id_data.begin(), account_id_data.begin() + length);
    }
    catch (const std::exception &e)
    {
        free(buffer_address);
        throw;
    }
}

std::wstring TCPGecko::get_mii_name()
{
    if (!is_connected())
        throw std::runtime_error("Not connected");

    constexpr size_t MiiNameSize = 11;
    constexpr size_t BufferSize = MiiNameSize * sizeof(uint16_t);
    constexpr uint32_t Alignment = 4;

    uint32_t buffer_address = malloc(BufferSize, Alignment);

    try
    {
        std::vector<uint8_t> zero_buffer(BufferSize, 0);
        upload_memory(buffer_address, zero_buffer);

        uint32_t func_addr = get_symbol("nn_act.rpl", "GetMiiName__Q2_2nn3actFPw");
        call(func_addr, {buffer_address});

        std::vector<uint8_t> mii_name_data = read_memory(buffer_address, BufferSize);

        std::wstring result;
        for (size_t i = 0; i + 1 < mii_name_data.size(); i += 2)
        {
            uint16_t ch = (mii_name_data[i] << 8) | mii_name_data[i + 1];
            if (ch == 0)
                break;
            result += (wchar_t) ch;
        }

        free(buffer_address);
        return result;
    }
    catch (const std::exception &e)
    {
        free(buffer_address);
        throw;
    }
}

std::string TCPGecko::get_server_version()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
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

uint32_t TCPGecko::get_persistent_id()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
    if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_ACCOUNT_IDENTIFIER;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char buffer[4];
    boost::asio::read(m_socket, boost::asio::buffer(buffer, 4));
    return read_u32_be(buffer);
}

uint32_t TCPGecko::get_os_version()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
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
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
    if (!is_connected())
        throw std::runtime_error("Not connected");

    char command = (char) COMMAND_GET_VERSION_HASH;
    boost::asio::write(m_socket, boost::asio::buffer(&command, 1));

    char response[4];
    boost::asio::read(m_socket, boost::asio::buffer(response, 4));
    return read_u32_be(response);
}

uint32_t TCPGecko::get_code_handler_address()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    
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
