#include "Config.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>


bool Config::load(const std::filesystem::path &config_path)
{
    std::ifstream in(config_path);

    m_loaded_file_path = config_path;

    if (!in)
        return false;
    
    try {
        in >> m_data;
    } catch (...) {
        return false;
    }

    return true;
}

bool Config::save(const std::filesystem::path &config_path) const
{
    if (empty())
        return false;
    
    std::filesystem::create_directories(config_path.parent_path());

    std::ofstream out(config_path);

    if (!out)
        return false;
    
    try {
        out << m_data.dump(4);
    } catch (...) {
        return false;
    }

    return true;
}

bool Config::save() const
{
    return save(m_loaded_file_path);
}
