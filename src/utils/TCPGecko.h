#include <vector>
#include <cstdint>
#include <cstring>
#include <string>

#include <boost/asio.hpp>


#define CODE_HANDLER_ENABLED_ADDRESS 0x10014CFC
#define CODE_LIST_START_ADDRESS 0x01133000


enum tcp_gecko_command : uint8_t
{
    COMMAND_WRITE_8 = 0x01,
    COMMAND_WRITE_16 = 0x02,
    COMMAND_WRITE_32 = 0x03,
    COMMAND_READ_MEMORY = 0x04,
    COMMAND_READ_MEMORY_KERNEL = 0x05,
    COMMAND_VALIDATE_ADDRESS_RANGE = 0x06,
    COMMAND_MEMORY_DISASSEMBLE = 0x08,
    COMMAND_READ_MEMORY_COMPRESSED = 0x09,
    COMMAND_KERNEL_WRITE = 0x0B,
    COMMAND_KERNEL_READ = 0x0C,
    COMMAND_TAKE_SCREEN_SHOT = 0x0D,
    COMMAND_UPLOAD_MEMORY = 0x41,
    COMMAND_SERVER_STATUS = 0x50,
    COMMAND_GET_DATA_BUFFER_SIZE = 0x51,
    COMMAND_READ_FILE = 0x52,
    COMMAND_READ_DIRECTORY = 0x53,
    COMMAND_REPLACE_FILE = 0x54,
    COMMAND_GET_CODE_HANDLER_ADDRESS = 0x55,
    COMMAND_READ_THREADS = 0x56,
    COMMAND_ACCOUNT_IDENTIFIER = 0x57,
    // COMMAND_WRITE_SCREEN = 0x58,
    COMMAND_FOLLOW_POINTER = 0x60,
    COMMAND_REMOTE_PROCEDURE_CALL = 0x70,
    COMMAND_GET_SYMBOL = 0x71,
    COMMAND_MEMORY_SEARCH_32 = 0x72,
    COMMAND_ADVANCED_MEMORY_SEARCH = 0x73,
    COMMAND_EXECUTE_ASSEMBLY = 0x81,
    COMMAND_PAUSE_CONSOLE = 0x82,
    COMMAND_RESUME_CONSOLE = 0x83,
    COMMAND_IS_CONSOLE_PAUSED = 0x84,
    COMMAND_SERVER_VERSION = 0x99,
    COMMAND_GET_OS_VERSION = 0x9A,
    COMMAND_SET_DATA_BREAKPOINT = 0xA0,
    COMMAND_SET_INSTRUCTION_BREAKPOINT = 0xA2,
    COMMAND_TOGGLE_BREAKPOINT = 0xA5,
    COMMAND_REMOVE_ALL_BREAKPOINTS = 0xA6,
    COMMAND_POKE_REGISTERS = 0xA7,
    COMMAND_GET_STACK_TRACE = 0xA8,
    COMMAND_GET_ENTRY_POINT_ADDRESS = 0xB1,
    COMMAND_RUN_KERNEL_COPY_SERVICE = 0xCD,
    COMMAND_IOSU_HAX_READ_FILE = 0xD0,
    COMMAND_GET_VERSION_HASH = 0xE0,
    COMMAND_PERSIST_ASSEMBLY = 0xE1,
    COMMAND_CLEAR_ASSEMBLY = 0xE2
};

/*
struct sys_information_t
{
    uint32_t bus_clock_speed;
    uint32_t core_clock_speed;
    uint32_t time_base;
    uint32_t l2_size[3];
    uint32_t cpu_ratio;
};
*/

class TCPGecko
{
private:
    boost::asio::io_context m_io_context;
    boost::asio::ip::tcp::socket m_socket;
    bool m_connected;
    
public:
    TCPGecko();
    ~TCPGecko();

    void connect(const std::string &ip_address, uint16_t port = 7331);
    void disconnect();

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
    uint32_t follow_pointer(uint32_t base_address, const std::vector<int32_t> &offsets);

    void upload_code_list(std::vector<uint8_t> data);
    void clear_code_list();
    void enable_code_handler(bool enabled);
    bool is_code_handler_enabled();

    size_t get_data_buffer_size();

    uint32_t get_symbol(const std::string &rplname, const std::string &symname, uint8_t data_flag = 0);
    uint64_t call(uint32_t address, const std::vector<uint32_t> &args = {}, int recv_size = 4);

    uint32_t malloc(uint32_t size, uint32_t alignment = 4);
    void free(uint32_t address);

    void set_game_mode_description(const std::wstring &description);
    void shutdown();
    uint64_t get_title_id();
    uint32_t get_principal_id();
    std::string get_account_id();
    std::wstring get_mii_name();
    std::string get_server_version();
    uint32_t get_persistent_id();
    uint32_t get_os_version();
    uint32_t get_version_hash();
    uint32_t get_code_handler_address();

    inline bool is_connected() const { return m_connected; }

    static bool valid_range(uint32_t address, uint32_t length);
    static bool valid_access(uint32_t address, uint32_t length, const std::string &access);

    static void write_u32(char *buf, uint32_t val);
    static void write_u16(char *buf, uint16_t val);
    static uint32_t read_u32_be(const char *buf);
    static uint64_t read_u64_be(const char *buf);
    static uint32_t read_u32_be(const std::vector<uint8_t> &data);
    static uint64_t read_u64_be(const std::vector<uint8_t> &data);
};
