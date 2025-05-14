#pragma once

#include <string>


class IRaimUITheme
{
private:
    std::string mName;
    std::string mDisplayName;
    
public:
    IRaimUITheme(std::string name)
        : mName(name), mDisplayName(name)
    {
    }

    void SetName(std::string name) { mName = name; }
    void SetDisplayName(std::string name) { mDisplayName = name; }

    const std::string &GetName() { return mName; }
    const std::string &GetDisplayName() { return mDisplayName; }
    
    virtual ~IRaimUITheme() = default;
    virtual void Apply() const;
};
