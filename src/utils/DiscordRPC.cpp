#include "DiscordRPC.h"

#include <unistd.h>
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

    void SendPacket(boost::asio::local::stream_protocol::socket& socket, int opcode, const json& data)
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
    std::string socket_path;
    // if (const char *runtime_dir = std::getenv("XDG_RUNTIME_DIR"))
    //     socket_path = std::string(runtime_dir) + "/discord-ipc-0";
    if (const char *tmp_dir = std::getenv("TMPDIR"))
        socket_path = std::string(tmp_dir) + "/discord-ipc-0";
    m_socket.connect(boost::asio::local::stream_protocol::endpoint(socket_path));

    SendHandshake();
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

    json activity = {
        {"cmd", "SET_ACTIVITY"},
        {"args", {
            {"pid", static_cast<int>(::getpid())},
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

        std::string socket_path;
        if (const char *tmp_dir = std::getenv("TMPDIR"))
            socket_path = std::string(tmp_dir) + "/discord-ipc-0";

        m_socket.connect(boost::asio::local::stream_protocol::endpoint(socket_path));
        SendHandshake();
    }
    catch (const std::exception& e)
    {
        std::cerr << "DiscordRPC Reconnect failed: " << e.what() << std::endl;
    }
}

void DiscordRPC::Update()
{
    try
    {
        SendActivity();
    }
    catch (const std::exception& e)
    {
        std::cerr << "DiscordRPC Update error: " << e.what() << std::endl;
        Reconnect();

        try
        {
            SendActivity();
        }
        catch (const std::exception& e)
        {
            std::cerr << "DiscordRPC Update retry failed: " << e.what() << std::endl;
        }
    }
    catch (...)
    {
        std::cerr << "DiscordRPC Update unknown error occurred." << std::endl;
    }
}

void DiscordRPC::SetPresence(const RichPresence& presence)
{
    m_rich_presence = presence;
}
