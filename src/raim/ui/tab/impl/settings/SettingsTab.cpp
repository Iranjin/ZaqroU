#include "SettingsTab.h"

#include "Setting.h"
#include <utils/Config.h>
#include <raim/ui/RaimUI.h>
#include <raim/Raim.h>
#include <raim/ui/RaimUI.h>

#include <imgui.h>


SettingsTab::SettingsTab(RaimUI *raimUI)
    : IRaimTab(raimUI, "Settings")
{
    AddSetting(TAB_APPEARANCE, "Theme", "theme", getRaimUI()->getUITheme()->AllThemes()[0]);
    AddSetting(TAB_APPEARANCE, "Font scale", "font_scale", 1.0f, 0.5f, 2.0f);
}

template <>
void SettingsTab::AddSetting<IRaimUITheme*>(TabType tab, const char* label, const char* key, IRaimUITheme* defaultValue)
{
    mTabSettings[tab].emplace_back(std::make_unique<Setting<std::string>>(
        label, key, defaultValue ? defaultValue->GetName() : "",
        [raimUI = getRaimUI()](const std::string &label, std::string &themeName) {
            RaimUI_Theme *uiTheme = raimUI->getUITheme();
            const auto &themes = uiTheme->AllThemes();

            std::vector<std::string> themeDisplayNames;

            int currentIndex = -1;

            for (int i = 0; i < themes.size(); ++i)
            {
                std::string name = themes[i]->GetName();
                std::string displayName = themes[i]->GetDisplayName();

                themeDisplayNames.push_back(displayName);

                if (name == themeName)
                    currentIndex = i;
            }

            bool changed = ImGui::Combo(("##" + label).c_str(), &currentIndex, 
                                        [](void *data, int idx, const char **out_text) -> bool {
                                            auto &themeDisplayNames = *reinterpret_cast<std::vector<std::string>*>(data);
                                            *out_text = themeDisplayNames[idx].c_str();
                                            return true;
                                        },
                                        &themeDisplayNames, (int) themeDisplayNames.size());

            if (changed && currentIndex >= 0)
            {
                themeName = themes[currentIndex]->GetName();
                themes[currentIndex]->Apply();
                return true;
            }

            return false;
        },
        [raimUI = getRaimUI()](const std::string& themeName) {
            for (IRaimUITheme *t : raimUI->getUITheme()->AllThemes())
                if (t->GetName() == themeName) t->Apply();
        }
    ));
}

template <>
void SettingsTab::AddSetting<bool>(TabType tab, const char *label, const char *key, bool defaultValue)
{
    mTabSettings[tab].emplace_back(std::make_unique<Setting<bool>>(
        label, key, defaultValue,
        [](const std::string &label, bool &value) {
            bool changed = ImGui::Checkbox(("##" + label).c_str(), &value);
            return changed;
        }
    ));
}

template <>
void SettingsTab::AddSetting<int>(TabType tab, const char *label, const char *key, int defaultValue, int minValue, int maxValue)
{
    mTabSettings[tab].emplace_back(std::make_unique<Setting<int>>(
        label, key, defaultValue,
        [minValue, maxValue](const std::string &label, int &value) {
            bool changed = ImGui::SliderInt(("##" + label).c_str(), &value, minValue, maxValue);
            return changed;
        }
    ));
}

template <>
void SettingsTab::AddSetting<float>(TabType tab, const char *label, const char *key, float defaultValue, float minValue, float maxValue)
{
    mTabSettings[tab].emplace_back(std::make_unique<Setting<float>>(
        label, key, defaultValue,
        [minValue, maxValue](const std::string &label, float &value) {
            bool changed = ImGui::SliderFloat(("##" + label).c_str(), &value, minValue, maxValue);
            return changed;
        }
    ));
}

template <>
void SettingsTab::AddSetting<double>(TabType tab, const char *label, const char *key, double defaultValue, double minValue, double maxValue)
{
    mTabSettings[tab].emplace_back(std::make_unique<Setting<double>>(
        label, key, defaultValue,
        [minValue, maxValue](const std::string &label, double &value) {
            bool changed = ImGui::SliderScalar(("##" + label).c_str(), ImGuiDataType_Double, &value, &minValue, &maxValue);
            return changed;
        }
    ));
}

void SettingsTab::OnTabOpened()
{
    std::shared_ptr<Config> config = getConfig();

    for (const auto &tab : mTabSettings)
    {
        for (const std::unique_ptr<ISetting> &setting : tab.second)
            setting->Load(config);
    }
}

void SettingsTab::Update()
{
    std::shared_ptr<Config> config = getConfig();
    bool changed = false;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 10));

    if (ImGui::BeginTabBar("SettingsTabBar"))
    {
        for (const auto &tab : mTabSettings)
        {
            const char *tabName = nullptr;
            switch (tab.first)
            {
                case TAB_GENERAL: tabName = "General"; break;
                case TAB_APPEARANCE: tabName = "Appearance"; break;
            }

            if (ImGui::BeginTabItem(tabName))
            {
                for (const std::unique_ptr<ISetting> &setting : tab.second)
                {
                    ImGui::PushID(setting.get());
                    ImGui::PushItemWidth(-1);
                    changed |= setting->Update(config);
                    ImGui::PopItemWidth();
                    ImGui::Spacing();
                    ImGui::PopID();
                }

                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    ImGui::PopStyleVar(2);

    if (changed)
        config->save();
}
