#include "CodesTab.h"

#include <filesystem>
#include <format>
#include <cstdint>

#include <utils/tcp_gecko/TCPGecko.h>
#include <utils/TitleIdParser.h>
#include <utils/Config.h>
#include <utils/common.h>
#include <raim/ui/tab/RaimTabManager.h>
#include <raim/ui/NotificationManager.h>
#include <raim/ui/tab/impl/codes/CodesTab.h>
#include <raim/ui/tab/IRaimTab.h>
#include <raim/ui/RaimUI.h>
#include <raim/Raim.h>
#include "backend/CodeLoader.h"

#include <imgui.h>


CodesTab::CodesTab(RaimUI *raimUI)
    : IRaimTab(raimUI, "Codes")
{
}

std::shared_ptr<TCPGecko> CodesTab::get_tcp_gecko()
{
    return get_raim()->get_tcp_gecko();
}

void CodesTab::CodesFrame_ScrollToIndex(size_t index, bool align_bottom)
{
    m_scroll_request.requested = true;
    m_scroll_request.target_index = index;
    m_scroll_request.align_bottom = align_bottom;
}

void CodesTab::SendCodes()
{
    std::shared_ptr<TCPGecko> tcp = get_tcp_gecko();

    if (!tcp->is_connected())
        return;
    
    std::vector<CodeEntry> cafe_codes = m_codes.filter_entries([](const CodeEntry entry) {
        return entry.assembly_ram_write == false && entry.enabled;
    });
    std::vector<CodeEntry> assembly_ram_writes = m_codes.filter_entries([](const CodeEntry &entry) {
        return entry.assembly_ram_write == true && entry.enabled;
    });

    bool has_written_codes = false;

    std::vector<uint8_t> cafe_code_values;

    for (CodeEntry &entry : cafe_codes)
    {
        std::string &lines = entry.codes;
        
        std::vector<uint32_t> values;

        std::istringstream iss(lines);
        std::string line;

        while (std::getline(iss, line))
        {
            if (line.empty())
                continue;
            
            std::istringstream line_stream(line);
            std::string addr_str;
            std::string val_str;

            if (line[0] != '#')
            {
                line_stream >> addr_str >> val_str;

                uint32_t left = std::stoul(addr_str, nullptr, 16);
                uint32_t right = std::stoul(val_str, nullptr, 16);

                values.push_back(left);
                values.push_back(right);
            }
        }

        cafe_code_values.reserve(values.size() * 4);

        for (uint32_t &value : values)
        {
            cafe_code_values.push_back((uint8_t) ((value >> 24) & 0xFF));
            cafe_code_values.push_back((uint8_t) ((value >> 16) & 0xFF));
            cafe_code_values.push_back((uint8_t) ((value >> 8) & 0xFF));
            cafe_code_values.push_back((uint8_t) (value & 0xFF));
        }

        has_written_codes |= !cafe_code_values.empty();
    }
    
    for (CodeEntry &entry : assembly_ram_writes)
    {
        std::string &lines = entry.codes;
        
        std::vector<uint32_t> addrWithoutHash;
        std::vector<uint32_t> valWithoutHash;

        std::istringstream iss(lines);
        std::string line;

        while (std::getline(iss, line))
        {
            if (line.empty())
                continue;
            
            std::istringstream line_stream(line);
            std::string addr_str;
            std::string val_str;

            if (line[0] != '#')
            {
                line_stream >> addr_str >> val_str;

                uint32_t address = std::stoul(addr_str, nullptr, 16);
                uint32_t value = std::stoul(val_str, nullptr, 16);

                addrWithoutHash.push_back(address);
                valWithoutHash.push_back(value);
            }
        }

        for (size_t i = 0; i < addrWithoutHash.size(); i++)
        {
            uint32_t &address = addrWithoutHash[i];
            uint32_t &value = valWithoutHash[i];

            tcp->write_mem_32(address, value);
        }

        has_written_codes |= !addrWithoutHash.empty();
    }

    if (!cafe_code_values.empty())
    {
        tcp->enable_code_handler(false);
        tcp->upload_code_list(cafe_code_values);
        tcp->enable_code_handler(true);
    }

    NotificationManager *notif_manager = get_raim_ui()->get_notification_manager();
    constexpr float notif_time = 3.0f;

    if (has_written_codes)
        notif_manager->AddNotification(m_notif_title, "Codes have been written to memory.", notif_time);
    else
        notif_manager->AddNotification(m_notif_title, "No codes to send.", notif_time);
}

void CodesTab::DisableCodes()
{
    std::shared_ptr<TCPGecko> tcp = get_tcp_gecko();

    if (!tcp->is_connected())
        return;
    
    std::vector<CodeEntry> cafe_codes = m_codes.filter_entries([](const CodeEntry entry) {
        return entry.assembly_ram_write == false && entry.enabled;
    });
    std::vector<CodeEntry> assembly_ram_writes = m_codes.filter_entries([](const CodeEntry &entry) {
        return entry.assembly_ram_write == true && entry.enabled;
    });

    std::vector<CodeEntry> codes;
    std::copy(cafe_codes.begin(),cafe_codes.end(),std::back_inserter(codes));
    std::copy(assembly_ram_writes.begin(),assembly_ram_writes.end(),std::back_inserter(codes));

    bool has_written_codes = false;
    
    for (CodeEntry &entry : codes)
    {
        std::string &lines = entry.codes;
        
        std::vector<uint32_t> addr_with_hash;
        std::vector<uint32_t> val_with_hash;

        std::istringstream iss(lines);
        std::string line;

        while (std::getline(iss, line))
        {
            if (line.empty())
                continue;
            
            std::istringstream line_stream(line);
            std::string addr_str;
            std::string val_str;

            if (line[0] == '#')
            {
                line_stream.get();
                line_stream >> addr_str >> val_str;

                uint32_t address = std::stoul(addr_str, nullptr, 16);
                uint32_t value = std::stoul(val_str, nullptr, 16);

                addr_with_hash.push_back(address);
                val_with_hash.push_back(value);
            }
        }

        for (size_t i = 0; i < addr_with_hash.size(); i++)
        {
            uint32_t &address = addr_with_hash[i];
            uint32_t &value = val_with_hash[i];

            tcp->write_mem_32(address, value);
        }

        has_written_codes |= !addr_with_hash.empty();
    }

    if (tcp->is_code_handler_enabled())
        has_written_codes = true;
    
    tcp->enable_code_handler(false);

    tcp->clear_code_list();

    NotificationManager *notif_manager = get_raim_ui()->get_notification_manager();
    constexpr float notif_time = 3.0f;

    if (has_written_codes)
        notif_manager->AddNotification(m_notif_title, "Codes have been disabled and memory was restored.", notif_time);
    else
        notif_manager->AddNotification(m_notif_title, "No codes to disable.", notif_time);
}

void CodesTab::OnConnected()
{
    std::shared_ptr<TCPGecko> tcp = get_tcp_gecko();

    uint64_t title_id = tcp->get_title_id();
    std::string title_id_str = TitleIdParser::from_Uint64(title_id);

    std::string codes_file_path = get_save_dir() + "/titles/" + title_id_str + "/codes.bin";

    if (std::filesystem::exists(codes_file_path))
    {
        m_loaded_path.clear();
        LoadCodes(codes_file_path);
    }
    else
    {
        m_codes.clear();
    }
    m_loaded_path = codes_file_path;
}

void CodesTab::SaveCodes(bool check_auto_save)
{
    if (m_loaded_path.empty())
        return;
    
    if (check_auto_save && !get_config()->get_nested("codes.code_list.auto_save", true))
        return;
    
    try
    {
        std::string folder_path = std::filesystem::path(m_loaded_path).parent_path().string();
        std::filesystem::create_directories(folder_path);

        CodeLoader::save_to_file(m_loaded_path, m_codes);
    }
    catch(const std::exception &e)
    {
        get_raim_ui()->get_notification_manager()->AddNotification(m_notif_title, e.what());
    }
}

void CodesTab::LoadCodes(const std::string &path, bool overwrite, bool save_path)
{
    NotificationManager *notif_mngr = get_raim_ui()->get_notification_manager();
    
    try
    {
        uint8_t version = CodeLoader::get_version_from_file(path);
        if (version > CODELOADER_VERSION)
        {
            notif_mngr->AddNotification(m_notif_title, "Versions are not compatible!");
            return;
        }
    
        m_selected_indices.clear();
        m_active_index = -1;

        if (overwrite)
            m_codes.clear();
        
        std::string ext = std::filesystem::path(path).extension().string();
        if (ext == ".xml")
            CodeLoader::load_from_xml_file(path, m_codes);
        else
            CodeLoader::load_from_file(path, m_codes);
        
        if (save_path)
            m_loaded_path = path;

        CodesFrame_ScrollToIndex(0, true);

        notif_mngr->AddNotification(m_notif_title, std::format("{} \"{}\"", overwrite ? "Loaded" : "Imported", path));
    }
    catch(const std::exception &e)
    {
        notif_mngr->AddNotification(m_notif_title, e.what());
        return;
    }
}
