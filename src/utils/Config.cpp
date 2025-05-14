#include "Config.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>


bool Config::load(const std::string& filename)
{
    std::ifstream in(filename);

    mLoadedFilePath = filename;

    if (!in)
        return false;
    
    try {
        in >> mData;
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
        out << mData.dump(4);
    } catch (...) {
        return false;
    }

    return true;
}

bool Config::save() const
{
    return save(mLoadedFilePath);
}
