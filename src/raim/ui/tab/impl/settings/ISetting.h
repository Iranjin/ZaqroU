#pragma once

#include <memory>


class Config;

class ISetting
{
public:
    virtual ~ISetting() = default;

    virtual void Save(std::shared_ptr<Config> config) = 0;
    virtual void Load(std::shared_ptr<Config> config) = 0;
    virtual bool Update(std::shared_ptr<Config> config) = 0;
    virtual void ResetToDefault() = 0;
};
