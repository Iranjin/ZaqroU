#pragma once

#include <string>


class IRaimUITheme
{
private:
    std::string m_name;
    std::string m_display_name;
    
public:
    IRaimUITheme(std::string name)
        : m_name(name), m_display_name(name)
    {
    }

    void set_name(std::string name) { m_name = name; }
    void set_display_name(std::string name) { m_display_name = name; }

    const std::string &get_name() { return m_name; }
    const std::string &get_display_name() { return m_display_name; }
    
    virtual ~IRaimUITheme() = default;
    virtual void apply() const;
};
