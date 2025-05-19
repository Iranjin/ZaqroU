#include "Config.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>


bool Config::load(const std::string& filename)
{
    std::ifstream in(filename);

    m_loaded_file_path = filename;

    if (!in)
        return false;
    
    try {
        in >> m_data;
    } catch (...) {
        return false;
    }

    return true;
}

bool Config::save(const std::string& filename) const
{
    if (empty())
        return false;
    
    std::filesystem::path path(filename);
    std::filesystem::create_directories(path.parent_path());

    std::ofstream out(filename);

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
