#include "NotificationManager.h"

#include <algorithm>
#include <cmath>


void NotificationManager::AddNotification(const std::string &title, const std::string &message, float time)
{
    const float notifHeight = 60.0f;
    const float padding = 10.0f;

    size_t index = mNotifications.size();
    float targetY = padding + (notifHeight + padding) * index;

    Notification notif = { title, message, time, 0.0f, targetY };
    mNotifications.push_back(notif);
}


void NotificationManager::Update()
{
    const float deltaTime = ImGui::GetIO().DeltaTime;
    ImVec2 viewportSize = ImGui::GetMainViewport()->Size;

    const float notifWidth = 300.0f;
    const float notifHeight = 60.0f;
    const float padding = 10.0f;

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

        float easedProgress = 1.0f - powf(1.0f - notif.animationProgress, 3.0f);  // EaseOutCubic

        float targetY = padding + (notifHeight + padding) * i;
        
        notif.currentY += (targetY - notif.currentY) * std::min(10.0f * deltaTime, 1.0f);

        ImVec2 pos = ImVec2((viewportSize.x - padding) - notifWidth * easedProgress + notifWidth, notif.currentY);
        ImVec2 messageSize = ImGui::CalcTextSize(notif.message.c_str());

        float adjustedWidth = std::max(300.0f, messageSize.x + padding * 2.0f);

        ImGui::SetNextWindowSize(ImVec2(adjustedWidth, notifHeight));
        ImGui::SetNextWindowPos(ImVec2(pos.x - adjustedWidth, notif.currentY));

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                                 ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

        ImGui::Begin(("##Notification" + std::to_string(i)).c_str(), nullptr, flags);

        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_CheckMark), "%s", notif.title.c_str());
        ImGui::Text("%s", notif.message.c_str());

        ImGui::End();
    }
}


