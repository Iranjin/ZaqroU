#include "LogsTab.h"

#include <iostream>
#include <deque>
#include <chrono>
#include <format>
#include <ctime>
#include <iomanip>
#include <sstream>

#include <utils/TCPGecko.h>
#include <raim/ui/NotificationManager.h>
#include <raim/ui/RaimUI.h>
#include <raim/Raim.h>

#include <imgui.h>


LogsTab::LogsTab(RaimUI *raimUI)
    : IRaimTab(raimUI, "Logs")
{
}

void LogsTab::Update()
{
    using namespace std::chrono;

    NotificationManager *notifManager = getRaimUI()->getNotificationManager();

    std::string logs;
    for (const NotificationManager::NotifLog &notif : notifManager->GetNotificationLogs())
    {
        std::time_t time = notif.timestamp;
        std::tm *localTime = std::localtime(&time);
        std::ostringstream ossTime;
        ossTime << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        std::string formattedTime = ossTime.str();

        std::string left = std::format("[{} : {}] ", notif.title, formattedTime);
        std::size_t leftWidth = left.size();

        std::istringstream messageStream(notif.message);
        std::string line;
        bool firstLine = true;
        while (std::getline(messageStream, line))
        {
            if (firstLine) {
                logs += left + line + '\n';
                firstLine = false;
            }
            else
            {
                logs += std::string(leftWidth, ' ') + line + '\n';
            }
        }
    }

    std::vector<char> logsBuffer(logs.begin(), logs.end());
    logsBuffer.push_back('\0');

    ImGui::InputTextMultiline("##Logs", logsBuffer.data(), logsBuffer.size(), 
        ImGui::GetContentRegionAvail(), ImGuiInputTextFlags_ReadOnly);
}
