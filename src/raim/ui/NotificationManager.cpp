#include "NotificationManager.h"

#include <algorithm>
#include <cmath>
#include <chrono>


void NotificationManager::AddNotification(const std::string &title, const std::string &message, float displayTime)
{
    const float padding = 10.0f;

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count() / 1000.0;

    mNotificationLogs.push_back({ title, message, timestamp });
    
    if (mNotificationLogs.size() > mMaxLogCount)
        mNotificationLogs.pop_front();

    Notification notif = { title, message, displayTime, 0.0f, 0.0f, padding };
    mNotifications.insert(mNotifications.begin(), notif);

    float currentY = padding;
    for (Notification &notif : mNotifications)
    {
        notif.currentY = currentY;
        currentY += notif.height + padding;
    }
}

void NotificationManager::Update()
{
    const float deltaTime = ImGui::GetIO().DeltaTime;
    ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
    const float padding = 10.0f;
    const float minNotifWidth = 300.0f;
    const float minNotifHeight = 60.0f;

    for (size_t i = mNotifications.size(); i-- > 0; )
    {
        Notification &notif = mNotifications[i];
        notif.timeRemaining -= deltaTime;

        if (notif.timeRemaining > 0.0f)
            notif.animationProgress = std::min(notif.animationProgress + deltaTime / mAnimationDuration, 1.0f);
        else
            notif.animationProgress = std::max(notif.animationProgress - deltaTime / mAnimationDuration, 0.0f);

        if (notif.timeRemaining <= 0.0f && notif.animationProgress <= 0.0f)
        {
            mNotifications.erase(mNotifications.begin() + i);
            continue;
        }

        ImVec2 titleSize = ImGui::CalcTextSize(notif.title.c_str());
        ImVec2 messageSize = ImGui::CalcTextSize(notif.message.c_str());
        
        float windowWidth = std::max(minNotifWidth, std::max(titleSize.x, messageSize.x) + 2 * padding);
        float contentHeight = titleSize.y + messageSize.y + 3 * padding;
        notif.height = std::max(minNotifHeight, contentHeight);

        float easedProgress = 1.0f - powf(1.0f - notif.animationProgress, 3.0f);  // EaseOutCubic

        float targetY = padding;
        for (size_t j = 0; j < i; ++j)
            targetY += mNotifications[j].height + padding;
        
        notif.currentY += (targetY - notif.currentY) * std::min(10.0f * deltaTime, 1.0f);

        ImVec2 pos = ImVec2((viewportSize.x - padding) - windowWidth * easedProgress + windowWidth, notif.currentY);

        ImGui::SetNextWindowSize(ImVec2(windowWidth, notif.height));
        ImGui::SetNextWindowPos(ImVec2(pos.x - windowWidth, notif.currentY));

        ImGui::SetNextWindowBgAlpha(0.8f);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | 
                                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | 
                                 ImGuiWindowFlags_NoNav;

        ImGui::Begin(("##Notification" + std::to_string(i)).c_str(), nullptr, flags);

        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_CheckMark), "%s", notif.title.c_str());
        
        ImGui::Dummy(ImVec2(0, padding/2));
        ImGui::TextUnformatted(notif.message.c_str());

        ImGui::End();
    }
}
