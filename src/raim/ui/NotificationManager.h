#pragma once

#include <string>
#include <vector>
#include <deque>
#include <imgui.h>

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
    struct Notification
    {
        std::string title;
        std::string message;
        float timeRemaining;
        float animationProgress;
        float height;
        float currentY = 0.0f;
    };

    std::vector<Notification> mNotifications;
    std::deque<NotifLog> mNotificationLogs;
    const float mAnimationDuration = 0.3f;
    const size_t mMaxLogCount = 1000;

public:
    void AddNotification(const std::string &title, const std::string &message, float displayTime = 5.0f);
    void Update();
    
    const std::deque<NotifLog> &GetNotificationLogs() const { return mNotificationLogs; }
    void ClearLogs() { mNotificationLogs.clear(); }
};
