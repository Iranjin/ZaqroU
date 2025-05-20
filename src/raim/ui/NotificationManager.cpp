#include "NotificationManager.h"

#include <imgui_internal.h>
#include <imgui.h>
#include <algorithm>
#include <cmath>
#include <chrono>


void NotificationManager::AddNotification(const std::string &title, const std::string &message, float display_time, std::function<void()> on_click)
{
    const float padding = 10.0f;

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count() / 1000.0;

    m_notification_logs.push_back({ title, message, timestamp });

    if (m_notification_logs.size() > m_max_log_count)
        m_notification_logs.pop_front();

    Notification notif = { title, message, NotifType::NOTIF_INFORMATION, display_time, 0.0f, 0.0f, padding, on_click };
    m_notifications.insert(m_notifications.begin(), notif);

    float currentY = padding;
    for (Notification &notif : m_notifications)
    {
        notif.current_y = currentY;
        currentY += notif.height + padding;
    }
}

void NotificationManager::AddWarnNotification(const std::string &title, const std::string &message, float display_time, std::function<void()> on_click)
{
    const float padding = 10.0f;

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count() / 1000.0;

    m_notification_logs.push_back({ title, message, timestamp });
    
    if (m_notification_logs.size() > m_max_log_count)
        m_notification_logs.pop_front();

    Notification notif = { title, message, NotifType::NOTIF_WARNING, display_time, 0.0f, 0.0f, padding, on_click };
    m_notifications.insert(m_notifications.begin(), notif);

    float current_y = padding;
    for (Notification &notif : m_notifications)
    {
        notif.current_y = current_y;
        current_y += notif.height + padding;
    }
}

void NotificationManager::AddErrorNotification(const std::string &title, const std::string &message, float display_time, std::function<void()> on_click)
{
    const float padding = 10.0f;

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count() / 1000.0;

    m_notification_logs.push_back({ title, message, timestamp });
    
    if (m_notification_logs.size() > m_max_log_count)
        m_notification_logs.pop_front();

    Notification notif = { title, message, NotifType::NOTIF_ERROR, display_time, 0.0f, 0.0f, padding, on_click };
    m_notifications.insert(m_notifications.begin(), notif);

    float current_y = padding;
    for (Notification &notif : m_notifications)
    {
        notif.current_y = current_y;
        current_y += notif.height + padding;
    }
}

void NotificationManager::Update()
{
    const float delta_time = ImGui::GetIO().DeltaTime;
    ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
    const float padding = 10.0f;
    const float minNotifWidth = 300.0f;
    const float minNotifHeight = 60.0f;

    float totalHeight = 0.0f;
    for (Notification &notif : m_notifications)
    {
        ImVec2 title_size = ImGui::CalcTextSize(notif.title.c_str());
        ImVec2 message_size = ImGui::CalcTextSize(notif.message.c_str());

        float contentHeight = title_size.y + message_size.y + 3 * padding;
        notif.height = std::max(minNotifHeight, contentHeight);

        totalHeight += notif.height + padding;
    }

    while (totalHeight > viewportSize.y && !m_notifications.empty())
    {
        totalHeight -= m_notifications.back().height + padding;
        m_notifications.back().remove();
    }

    for (size_t i = m_notifications.size(); i-- > 0; )
    {
        Notification &notif = m_notifications[i];
        notif.time_remaining -= delta_time;

        if (notif.time_remaining > 0.0f)
            notif.animation_progress = std::min(notif.animation_progress + delta_time / m_animation_duration, 1.0f);
        else
            notif.animation_progress = std::max(notif.animation_progress - delta_time / m_animation_duration, 0.0f);

        if (notif.time_remaining <= 0.0f && notif.animation_progress <= 0.0f)
        {
            m_notifications.erase(m_notifications.begin() + i);
            continue;
        }

        ImVec2 title_size = ImGui::CalcTextSize(notif.title.c_str());
        ImVec2 message_size = ImGui::CalcTextSize(notif.message.c_str());
        
        float window_width = std::max(minNotifWidth, std::max(title_size.x, message_size.x) + 2 * padding);
        float content_height = title_size.y + message_size.y + 3 * padding;
        notif.height = std::max(minNotifHeight, content_height);

        float eased_progress = 1.0f - powf(1.0f - notif.animation_progress, 3.0f);  // EaseOutCubic

        float targetY = padding;
        for (size_t j = 0; j < i; ++j)
            targetY += m_notifications[j].height + padding;
        
        notif.current_y += (targetY - notif.current_y) * std::min(10.0f * delta_time, 1.0f);

        ImVec2 pos = ImVec2((viewportSize.x - padding) - window_width * eased_progress + window_width, notif.current_y);

        ImGui::SetNextWindowSize(ImVec2(window_width, notif.height));
        ImGui::SetNextWindowPos(ImVec2(pos.x - window_width, notif.current_y));

        ImGui::SetNextWindowBgAlpha(0.8f);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | 
                                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | 
                                 ImGuiWindowFlags_NoNav;

        ImGui::Begin(("##Notification" + std::to_string(i)).c_str(), nullptr, flags);

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            if (notif.on_click)
                notif.on_click();
            notif.remove();
        }

        ImVec4 color;
        switch (notif.type)
        {
        case (NotifType::NOTIF_INFORMATION): color = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled); break;
        case (NotifType::NOTIF_WARNING): color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); break;
        case (NotifType::NOTIF_ERROR): color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); break;
        }
        
        ImGui::TextColored(color, "%s", notif.title.c_str());
        
        ImGui::Dummy(ImVec2(0, padding / 2));
        ImGui::TextUnformatted(notif.message.c_str());

        ImGui::End();
    }
}
