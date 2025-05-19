#pragma once

#include <memory>
#include <chrono>
#include <future>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>

#include <GLFW/glfw3.h>


class Config;
class RaimUI;
class TitleIdParser;
class TCPGecko;
class DiscordRPC;

class Raim
{
private:
    GLFWwindow *m_window;
    RaimUI *m_raim_ui;
    std::shared_ptr<Config> m_config;
    std::shared_ptr<TCPGecko> m_tcp_gecko;
    std::shared_ptr<TitleIdParser> m_title_id_parser;
    std::string m_config_path;

    // Discord RPC
    std::shared_ptr<DiscordRPC> m_discord_rpc;
    std::atomic<bool> m_rpc_thread_running{true};
    std::thread m_rpc_thread;
    std::mutex m_rpc_thread_mutex;
    std::condition_variable m_rpc_thread_cv;
    
public:
    Raim(GLFWwindow *window);
    ~Raim();

    void Update();

    void LoadFonts();
    void LoadTitles();

    void Discord_RPC_Update();

    RaimUI* get_raim_ui() const { return m_raim_ui; }
    std::shared_ptr<Config> get_config() const { return m_config; }
    std::shared_ptr<TCPGecko> get_tcp_gecko() const { return m_tcp_gecko; }
    std::string get_config_path() const { return m_config_path; }
    GLFWwindow *get_window() const { return m_window; }
};
