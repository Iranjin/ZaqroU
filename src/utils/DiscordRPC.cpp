#include "DiscordRPC.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <cstdlib>
#include <iostream>
#include <random>


using json = nlohmann::json;


namespace
{
    constexpr int OPCODE_HANDSHAKE = 0;
    constexpr int OPCODE_FRAME = 1;

    std::string GenerateNonce()
    {
        static std::mt19937 rng(std::random_device{}());
        static std::uniform_int_distribution<> dist(0, 15);
        std::string nonce;
        for (int i = 0; i < 16; ++i)
            nonce += "0123456789abcdef"[dist(rng)];
        return nonce;
    }

    void SendPacket(boost::asio::local::stream_protocol::socket &socket, int opcode, const json &data)
    {
        std::string payload = data.dump();
        uint32_t length = payload.size();

        std::vector<char> buffer(8 + length);
        std::memcpy(buffer.data(), &opcode, 4);
        std::memcpy(buffer.data() + 4, &length, 4);
        std::memcpy(buffer.data() + 8, payload.data(), length);

        boost::asio::write(socket, boost::asio::buffer(buffer));
    }
}

DiscordRPC::DiscordRPC(const std::string &client_id)
    : m_socket(m_io_context), m_client_id(client_id)
{
    Connect();
}

void DiscordRPC::Connect()
{
    std::string socket_path;

    #ifdef __APPLE__
    if (const char *tmp_dir = std::getenv("TMPDIR"))
        socket_path = std::string(tmp_dir) + "/discord-ipc-0";
#elif defined(__linux__)
    const char *tmp_dirs[] = {
        std::getenv("XDG_RUNTIME_DIR"),
        std::getenv("TMPDIR"),
        std::getenv("TMP"),
        std::getenv("TEMP"),
        "/tmp"
    };

    const char *patterns[] = {
        "%s/discord-ipc-%d",
        "%s/app/com.discordapp.Discord/discord-ipc-%d",
        "%s/snap.discord-canary/discord-ipc-%d",
        "%s/snap.discord/discord-ipc-%d"
    };

    for (const char *dir : tmp_dirs)
    {
        if (!dir)
            continue;

        for (const char *pattern : patterns)
        {
            for (int i = 0; i < 10; ++i)
            {
                char path[108];
                std::snprintf(path, sizeof(path), pattern, dir, i);
                if (access(path, F_OK) == 0)
                {
                    socket_path = path;
                    goto found;
                }
            }
        }
    }
found:;
#endif
    
    if (!socket_path.empty())
    {
        try
        {
            m_socket.connect(boost::asio::local::stream_protocol::endpoint(socket_path));
            SendHandshake();
        }
        catch(const std::exception &e) { }
    }
}

void DiscordRPC::SendHandshake()
{
    json handshake = {
        {"v", 1},
        {"client_id", m_client_id}
    };
    SendPacket(m_socket, OPCODE_HANDSHAKE, handshake);
}

void DiscordRPC::SendActivity()
{
    json activity_data;

    if (!m_rich_presence.state.empty())
        activity_data["state"] = m_rich_presence.state;

    if (!m_rich_presence.details.empty())
        activity_data["details"] = m_rich_presence.details;

    if (m_rich_presence.start_time > 0)
        activity_data["timestamps"] = {
            {"start", m_rich_presence.start_time}
        };
    
    int pid =
#ifdef _WIN32
        static_cast<int>(GetCurrentProcessId());
#else
        static_cast<int>(::getpid());
#endif

    json activity = {
        {"cmd", "SET_ACTIVITY"},
        {"args", {
            {"pid", pid},
            {"activity", activity_data}
        }},
        {"nonce", GenerateNonce()}
    };

    SendPacket(m_socket, OPCODE_FRAME, activity);
}

void DiscordRPC::Reconnect()
{
    try
    {
        m_socket.close();
        Connect();
    }
    catch (const std::exception &e) { }
}

void DiscordRPC::Update()
{
    try
    {
        SendActivity();
    }
    catch (const std::exception &e)
    {
        Reconnect();

        try
        {
            SendActivity();
        }
        catch (const std::exception &e) { }
    }
    catch (...) { }
}

void DiscordRPC::SetPresence(const RichPresence &presence)
{
    m_rich_presence = presence;
}
