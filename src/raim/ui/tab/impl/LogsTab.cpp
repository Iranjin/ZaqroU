#include "LogsTab.h"

#include <iostream>
#include <deque>
#include <chrono>
#include <format>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <utils/tcp_gecko/TCPGecko.h>
#include <raim/ui/NotificationManager.h>
#include <raim/ui/RaimUI.h>
#include <raim/Raim.h>

#include <imgui.h>


LogsTab::LogsTab(RaimUI *raim_ui)
    : IRaimTab(raim_ui, "Logs")
{
}

void LogsTab::Update()
{
    using namespace std::chrono;

    NotificationManager *notif_manager = get_raim_ui()->get_notification_manager();

    std::string logs;
    for (const NotificationManager::NotifLog &notif : notif_manager->GetNotificationLogs())
    {
        std::time_t time = notif.timestamp;
        std::tm *local_time = std::localtime(&time);
        std::ostringstream oss_time;
        oss_time << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");
        std::string formatted_time = oss_time.str();

        std::string left = std::format("[{} : {}] ", notif.title, formatted_time);
        std::size_t left_width = left.size();

        std::istringstream message_stream(notif.message);
        std::string line;
        bool first_line = true;
        while (std::getline(message_stream, line))
        {
            if (first_line) {
                logs += left + line + '\n';
                first_line = false;
            }
            else
            {
                logs += std::string(left_width, ' ') + line + '\n';
            }
        }
    }

    std::vector<char> logs_buffer(logs.begin(), logs.end());
    logs_buffer.push_back('\0');

    ImGui::InputTextMultiline("##Logs", logs_buffer.data(), logs_buffer.size(), 
        ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_ReadOnly);
}
