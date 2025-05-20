#pragma once

#include <string>
#include <vector>
#include <deque>
#include <functional>


class NotificationManager
{
public:
    struct NotifLog
    {
        std::string title;
        std::string message;
        double timestamp;
    };

private:
    using OnClickEvent = std::function<void()>;

    enum class NotifType
    {
        INFORMATION,
        WARNING,
        ERROR
    };

    struct Notification
    {
        std::string title;
        std::string message;
        NotifType type;
        float time_remaining;
        float animation_progress;
        float height;
        float current_y = 0.0f;
        OnClickEvent on_click{};

        void remove() { time_remaining = 0.0f; };
    };

    std::vector<Notification> m_notifications;
    std::deque<NotifLog> m_notification_logs;
    const float m_animation_duration = 0.3f;
    const size_t m_max_log_count = 1000;

public:
    void AddNotification(const std::string &title, const std::string &message, float display_time = 5.0f, OnClickEvent on_click = OnClickEvent());
    void AddWarnNotification(const std::string &title, const std::string &message, float display_time = 5.0f, OnClickEvent on_click = OnClickEvent());
    void AddErrorNotification(const std::string &title, const std::string &message, float display_time = 5.0f, OnClickEvent on_click = OnClickEvent());

    void Update();
    
    const std::deque<NotifLog> &GetNotificationLogs() const { return m_notification_logs; }
    void ClearLogs() { m_notification_logs.clear(); }
};
