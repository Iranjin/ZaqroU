#include "Raim.h"

#include <cstdint>
#include <format>

#include <imgui.h>

#include <constants.h>
#include <utils/tcp_gecko/TCPGecko.h>
// #include <utils/StrUtils.h>
#include <utils/Config.h>
#include <utils/downloader.h>
#include <utils/common.h>
#include <utils/TitleIdParser.h>
#include <utils/DiscordRPC.h>
#include "ui/tab/RaimTabManager.h"
#include "ui/tab/IRaimTab.h"
#include "ui/RaimUI.h"
#include "ui/style/RaimUI_Theme.h"

#include "HackNerdFont_Regular_ttf.h"
#include "NotoSansJP_Regular_ttf.h"


Raim::Raim(GLFWwindow *window)
    : m_window(window),
      m_config(std::make_shared<Config>()),
      m_tcp_gecko(std::make_shared<TCPGecko>()),
      m_title_id_parser(std::make_shared<TitleIdParser>(get_save_dir() + "/res/titles.xml")),
      m_discord_rpc(std::make_shared<DiscordRPC>(DISCORD_RPC_CLIENT_ID)),
      m_config_path(get_save_dir() + "/config.json")
{
    m_config->load(m_config_path);
    LoadFonts();
    m_raim_ui = new RaimUI(this);
    m_raim_ui->Init();
    LoadTitles();

    m_rpc_thread = std::thread(&Raim::Discord_RPC_Update, this);
}

Raim::~Raim()
{
    delete m_raim_ui;

    m_rpc_thread_running = false;
    m_rpc_thread_cv.notify_all();
    if (m_rpc_thread.joinable())
        m_rpc_thread.join();
}

void Raim::LoadFonts()
{
    ImGuiIO &io = ImGui::GetIO();

    ImFontConfig font_config;

    font_config.FontDataOwnedByAtlas = false;

    { // English font
        font_config.MergeMode = false;

        static ImWchar const glyph_ranges[] = {
            0x0020, 0xfffd,
            0,
        };

        io.Fonts->AddFontFromMemoryTTF(
            (void *) HackNerdFont_Regular_ttf, 
            HackNerdFont_Regular_ttf_len,
            32.0f, 
            &font_config,
            glyph_ranges
        );
    }

    { // Japanese font
        font_config.MergeMode = true;

        io.Fonts->AddFontFromMemoryTTF(
            (void *) NotoSansJP_Regular_ttf,
            NotoSansJP_Regular_ttf_len,
            44.0f,
            &font_config,
            io.Fonts->GetGlyphRangesJapanese()
        );
    }

    io.FontGlobalScale = 0.50f;
}

void Raim::LoadTitles()
{
    std::string titles_path = m_title_id_parser->get_path();
    
    if (!std::filesystem::exists(m_title_id_parser->get_path()))
    {
        std::vector<char> data;
        std::string titles_url = std::format("{}/res/titles.xml", FILE_SERVER_URL);
        download_file(titles_url, data);
        const std::string folder_parent_path = std::filesystem::path(titles_path).parent_path();
        std::filesystem::create_directories(folder_parent_path);
        save_to_file(titles_path, data);
    }
    m_title_id_parser->load();
}

void Raim::Discord_RPC_Update()
{
    std::unique_lock<std::mutex> lock(m_rpc_thread_mutex);

    while (m_rpc_thread_running.load())
    {
        lock.unlock();

#ifndef _WIN32
        DiscordRPC::RichPresence rp;
        static int32_t start_time = std::time(nullptr);

        if (auto tcp = get_tcp_gecko(); tcp->is_connected())
        {
            uint64_t title_id = 0;
            try { title_id = tcp->get_title_id(); }
            catch (const std::exception &e) { }

            if (const TitleEntry *entry = m_title_id_parser->find_entry_by_Uint64(title_id); title_id != 0)
                rp.details = std::format("Playing: {}", entry->description);
        }

        if (IRaimTab* current_tab = get_raim_ui()->get_tab_manager()->get_active_tab())
            rp.state = std::format("📁 - {}", current_tab->get_tab_name());

        rp.start_time = start_time;

        m_discord_rpc->SetPresence(rp);
        m_discord_rpc->Update();
#endif

        lock.lock();
        m_rpc_thread_cv.wait_for(lock, std::chrono::seconds(10), [this] {
            return !m_rpc_thread_running.load();
        });
    }
}

void Raim::Update()
{
    m_raim_ui->Update();
}
