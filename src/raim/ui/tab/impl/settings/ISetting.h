#pragma once

#include <memory>


class Config;

class ISetting
{
public:
    virtual ~ISetting() = default;

    virtual void save(std::shared_ptr<Config> config) = 0;
    virtual void load(std::shared_ptr<Config> config) = 0;
    virtual bool update(std::shared_ptr<Config> config) = 0;
    virtual void reset_to_default() = 0;
};
