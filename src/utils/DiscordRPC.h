#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>


class DiscordRPC
{
public:
    struct RichPresence
    {
        std::string state;
        std::string details;
        std::int64_t start_time = 0;
    };

private:
    boost::asio::io_context m_io_context;
    boost::asio::local::stream_protocol::socket m_socket;
    RichPresence m_rich_presence;
    std::string m_client_id;

    void Connect();
    void SendHandshake();
    void SendActivity();

public:
    explicit DiscordRPC(const std::string &client_id);
    void Reconnect();
    void Update();
    void SetPresence(const RichPresence &presence);
};
