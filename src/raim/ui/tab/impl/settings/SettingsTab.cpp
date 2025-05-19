#include "SettingsTab.h"

#include "Setting.h"
#include <utils/Config.h>
#include <raim/ui/RaimUI.h>
#include <raim/Raim.h>
#include <raim/ui/RaimUI.h>

#include <imgui.h>


SettingsTab::SettingsTab(RaimUI *raim_ui)
    : IRaimTab(raim_ui, "Settings")
{
    AddSetting(TAB_APPEARANCE, "Theme", "appearance.theme", get_raim_ui()->get_ui_theme()->all_themes()[0]);
    AddSetting(TAB_APPEARANCE, "Font scale", "appearance.font_scale", 1.0f, 0.5f, 2.0f);
}

template <>
void SettingsTab::AddSetting<IRaimUITheme*>(TabType tab, const char *label, const char *key, IRaimUITheme *defaultValue)
{
    mTabSettings[tab].emplace_back(std::make_unique<Setting<std::string>>(
        label, key, defaultValue ? defaultValue->get_name() : "",
        [raim_ui = get_raim_ui()](const std::string &label, std::string &theme_name) {
            RaimUI_Theme *uiTheme = raim_ui->get_ui_theme();
            const auto &themes = uiTheme->all_themes();

            std::vector<std::string> theme_display_names;

            int current_index = -1;

            for (int i = 0; i < themes.size(); ++i)
            {
                std::string name = themes[i]->get_name();
                std::string displayName = themes[i]->get_display_name();

                theme_display_names.push_back(displayName);

                if (name == theme_name)
                    current_index = i;
            }

            bool changed = ImGui::Combo(("##" + label).c_str(), &current_index, 
                                        [](void *data, int idx, const char **out_text) -> bool {
                                            auto &theme_display_names = *reinterpret_cast<std::vector<std::string>*>(data);
                                            *out_text = theme_display_names[idx].c_str();
                                            return true;
                                        },
                                        &theme_display_names, (int) theme_display_names.size());

            if (changed && current_index >= 0)
            {
                theme_name = themes[current_index]->get_name();
                themes[current_index]->apply();
                return true;
            }

            return false;
        },
        [raim_ui = get_raim_ui()](const std::string &theme_name) {
            for (IRaimUITheme *t : raim_ui->get_ui_theme()->all_themes())
                if (t->get_name() == theme_name) t->apply();
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
    std::shared_ptr<Config> config = get_config();

    for (const auto &tab : mTabSettings)
    {
        for (const std::unique_ptr<ISetting> &setting : tab.second)
            setting->load(config);
    }
}

void SettingsTab::Update()
{
    std::shared_ptr<Config> config = get_config();
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
                case TAB_CONNECTION: tabName = "Connection"; break;
                case TAB_APPEARANCE: tabName = "Appearance"; break;
            }

            if (ImGui::BeginTabItem(tabName))
            {
                for (const std::unique_ptr<ISetting> &setting : tab.second)
                {
                    ImGui::PushID(setting.get());
                    ImGui::PushItemWidth(-1);
                    changed |= setting->update(config);
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
