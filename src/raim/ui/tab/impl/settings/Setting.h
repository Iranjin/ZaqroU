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
    Setting(const std::string& label, const std::string& key, T defaultValue,
            std::function<bool(const std::string&, T&)> uiFunc,
            std::function<void(const T&)> onReset = nullptr)
        : mLabel(label), mKey(key), mDefaultValue(defaultValue), mValue(defaultValue),
          mUIFunc(uiFunc), mOnReset(onReset)
    {
    }

    void Save(std::shared_ptr<Config> config) override
    {
        config->set(mKey, mValue);
    }

    void Load(std::shared_ptr<Config> config) override
    {
        mValue = config->get(mKey, mValue);
    }

    bool Update(std::shared_ptr<Config> config) override
    {
        ImGui::SeparatorText(mLabel.c_str());
        
        T old = mValue;
        if (mUIFunc(mLabel, mValue))
        {
            if (old != mValue)
            {
                Save(config);
                return true;
            }
        }
        
        if (IsModified() && ImGui::Button("Reset"))
        {
            ResetToDefault();
            Save(config);
            return true;
        }
        
        return false;
    }

    void ResetToDefault() override
    {
        mValue = mDefaultValue;

        if (mOnReset)
            mOnReset(mValue);
    }

    bool IsModified() const
    {
        return mValue != mDefaultValue;
    }

private:
    std::string mLabel;
    std::string mKey;
    T mDefaultValue;
    T mValue;
    std::function<bool(const std::string&, T&)> mUIFunc;
    std::function<void(const T&)> mOnReset;
};
