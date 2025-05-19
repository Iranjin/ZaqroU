#pragma once

#include <vector>
#include <unordered_map>

#include <raim/ui/style/RaimUI_Theme.h>
#include <raim/ui/tab/IRaimTab.h>


class ISetting;

class SettingsTab : public IRaimTab
{
private:
    enum TabType
    {
        TAB_GENERAL,
        TAB_CONNECTION,
        TAB_APPEARANCE,
    };

    std::unordered_map<TabType, std::vector<std::unique_ptr<ISetting>>> mTabSettings;

public:
    SettingsTab(RaimUI *raimUI);

    template<typename T>
    void AddSetting(TabType tab, const char *label, const char *key, T defaultValue);

    template<typename T>
    void AddSetting(TabType tab, const char *label, const char *key, T defaultValue, T minValue, T maxValue);

    void UpdateOption();
    
    void OnTabOpened() override;
    void Update() override;
};

template<>
void SettingsTab::AddSetting<IRaimUITheme*>(TabType tab, const char* label, const char* key, IRaimUITheme* defaultValue);

template<>
void SettingsTab::AddSetting<bool>(TabType tab, const char *label, const char *key, bool defaultValue);

template<>
void SettingsTab::AddSetting<int>(TabType tab, const char *label, const char *key, int defaultValue, int minValue, int maxValue);

template<>
void SettingsTab::AddSetting<float>(TabType tab, const char *label, const char *key, float defaultValue, float minValue, float maxValue);

template<>
void SettingsTab::AddSetting<double>(TabType tab, const char *label, const char *key, double defaultValue, double minValue, double maxValue);
