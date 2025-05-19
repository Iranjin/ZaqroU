#pragma once

#include <functional>
#include <string>

#include <utils/Config.h>
#include "ISetting.h"

#include <imgui.h>


template <typename T>
class Setting : public ISetting
{
public:
    Setting(const std::string& label, const std::string& key, T default_value,
            std::function<bool(const std::string&, T&)> ui_func,
            std::function<void(const T&)> on_reset = nullptr)
        : m_label(label), m_key(key), m_default_value(default_value), m_value(default_value),
          m_ui_func(ui_func), m_on_reset(on_reset)
    {
    }

    void save(std::shared_ptr<Config> config) override
    {
        config->set_nested(m_key, m_value);
    }

    void load(std::shared_ptr<Config> config) override
    {
        m_value = config->get_nested(m_key, m_value);
    }

    bool update(std::shared_ptr<Config> config) override
    {
        ImGui::SeparatorText(m_label.c_str());
        
        T old = m_value;
        if (m_ui_func(m_label, m_value))
        {
            if (old != m_value)
            {
                save(config);
                return true;
            }
        }
        
        if (IsModified() && ImGui::Button("Reset"))
        {
            reset_to_default();
            save(config);
            return true;
        }
        
        return false;
    }

    void reset_to_default() override
    {
        m_value = m_default_value;

        if (m_on_reset)
            m_on_reset(m_value);
    }

    bool IsModified() const
    {
        return m_value != m_default_value;
    }

private:
    std::string m_label;
    std::string m_key;
    T m_default_value;
    T m_value;
    std::function<bool(const std::string&, T&)> m_ui_func;
    std::function<void(const T&)> m_on_reset;
};
