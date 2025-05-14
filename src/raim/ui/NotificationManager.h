#pragma once

#include <string>
#include <vector>
#include <imgui.h>


class NotificationManager
{
private:
    struct Notification
    {
        std::string title;
        std::string message;
        float timeRemaining;
        float animationProgress;
        float currentY = 0.0f;
    };

    std::vector<Notification> mNotifications;
    const float mAnimationDuration = 0.3f;

public:
    void AddNotification(const std::string &title, const std::string &message, float time = 5.0f);
    void Update();
};
