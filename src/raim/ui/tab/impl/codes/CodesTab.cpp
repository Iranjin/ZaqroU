#include "CodesTab.h"

#include <filesystem>

#include <utils/TCPGecko.h>
#include <utils/TitleIdParser.h>
#include <raim/ui/NotificationManager.h>
#include "../../../RaimUI.h"
#include "../../../../Raim.h"

#include <imgui.h>


CodesTab::CodesTab(RaimUI *raimUI)
    : IRaimTab(raimUI, "Codes")
{
}

std::shared_ptr<TCPGecko> CodesTab::getTCPGecko()
{
    return getRaim()->getTCPGecko();
}

void CodesTab::SendCodes()
{
    std::shared_ptr<TCPGecko> tcp = getTCPGecko();

    if (!tcp->is_connected())
        return;
    
    std::vector<CodeEntry> cafe_codes = mCodes.filter_entries([](const CodeEntry entry) {
        return entry.assemblyRamWrite == false && entry.enabled;
    });
    std::vector<CodeEntry> assembly_ram_writes = mCodes.filter_entries([](const CodeEntry &entry) {
        return entry.assemblyRamWrite == true && entry.enabled;
    });

    bool hasWrittenCodes = false;

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
            
            std::istringstream lineStream(line);
            std::string addrStr;
            std::string valStr;

            if (line[0] != '#')
            {
                lineStream >> addrStr >> valStr;

                uint32_t left = std::stoul(addrStr, nullptr, 16);
                uint32_t right = std::stoul(valStr, nullptr, 16);

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

        hasWrittenCodes |= !cafe_code_values.empty();
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
            
            std::istringstream lineStream(line);
            std::string addrStr;
            std::string valStr;

            if (line[0] != '#')
            {
                lineStream >> addrStr >> valStr;

                uint32_t address = std::stoul(addrStr, nullptr, 16);
                uint32_t value = std::stoul(valStr, nullptr, 16);

                addrWithoutHash.push_back(address);
                valWithoutHash.push_back(value);
            }
        }

        for (size_t i = 0; i < addrWithoutHash.size(); i++)
        {
            uint32_t &address = addrWithoutHash[i];
            uint32_t &value = valWithoutHash[i];

            tcp->write_memory(address, value);
        }

        hasWrittenCodes |= !addrWithoutHash.empty();
    }

    if (!cafe_code_values.empty())
    {
        tcp->enable_code_handler(false);
        tcp->upload_code_list(cafe_code_values);
        tcp->enable_code_handler(true);
    }

    NotificationManager *notifManager = getRaimUI()->GetNotificationManager();
    constexpr float notifTime = 3.0f;

    if (hasWrittenCodes)
        notifManager->AddNotification(mNotifTitle, "Codes have been written to memory.", notifTime);
    else
        notifManager->AddNotification(mNotifTitle, "No codes to send.", notifTime);
}

void CodesTab::DisableCodes()
{
    std::shared_ptr<TCPGecko> tcp = getTCPGecko();

    if (!tcp->is_connected())
        return;
    
    std::vector<CodeEntry> cafe_codes = mCodes.filter_entries([](const CodeEntry entry) {
        return entry.assemblyRamWrite == false && entry.enabled;
    });
    std::vector<CodeEntry> assembly_ram_writes = mCodes.filter_entries([](const CodeEntry &entry) {
        return entry.assemblyRamWrite == true && entry.enabled;
    });

    std::vector<CodeEntry> codes;
    std::copy(cafe_codes.begin(),cafe_codes.end(),std::back_inserter(codes));
    std::copy(assembly_ram_writes.begin(),assembly_ram_writes.end(),std::back_inserter(codes));

    bool hasWrittenCodes = false;
    
    for (CodeEntry &entry : codes)
    {
        std::string &lines = entry.codes;
        
        std::vector<uint32_t> addrWithHash;
        std::vector<uint32_t> valWithHash;

        std::istringstream iss(lines);
        std::string line;

        while (std::getline(iss, line))
        {
            if (line.empty())
                continue;
            
            std::istringstream lineStream(line);
            std::string addrStr;
            std::string valStr;

            if (line[0] == '#')
            {
                lineStream.get();
                lineStream >> addrStr >> valStr;

                uint32_t address = std::stoul(addrStr, nullptr, 16);
                uint32_t value = std::stoul(valStr, nullptr, 16);

                addrWithHash.push_back(address);
                valWithHash.push_back(value);
            }
        }

        for (size_t i = 0; i < addrWithHash.size(); i++)
        {
            uint32_t &address = addrWithHash[i];
            uint32_t &value = valWithHash[i];

            tcp->write_memory(address, value);
        }

        hasWrittenCodes |= !addrWithHash.empty();
    }

    if (tcp->is_code_handler_enabled())
        hasWrittenCodes = true;
    
    tcp->enable_code_handler(false);

    NotificationManager *notifManager = getRaimUI()->GetNotificationManager();
    constexpr float notifTime = 3.0f;

    if (hasWrittenCodes)
        notifManager->AddNotification(mNotifTitle, "Codes have been disabled and memory was restored.", notifTime);
    else
        notifManager->AddNotification(mNotifTitle, "No codes to disable.", notifTime);
}

void CodesTab::OnConnected()
{
    std::shared_ptr<TCPGecko> tcp = getTCPGecko();

    uint64_t title_id = tcp->get_title_id();
    std::string title_id_str = TitleIdParser::fromUint64(title_id);

    std::string codes_file_path = "zaqro_u/titles/" + title_id_str + "/codes.bin";

    if (std::filesystem::exists(codes_file_path))
    {
        mCodes.clear();
        mLoadedPath.clear();
        mSelectedIndices.clear();
        mActiveIndex = -1;
        CodeLoader::LoadFromFile(codes_file_path, mCodes);

        getRaimUI()->GetNotificationManager()->AddNotification("CodesTab", std::format("Loaded \"{}\"", codes_file_path));
    }
    mLoadedPath = codes_file_path;
}
