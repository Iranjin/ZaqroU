#include "common.h"

#include <cstdlib>
#include <cctype>


std::filesystem::path get_home_dir()
{
#ifdef _WIN32
    return std::getenv("USERPROFILE");
#else
    return std::getenv("HOME");
#endif
}

std::filesystem::path get_save_dir()
{
    std::filesystem::path home_dir = get_home_dir();
    home_dir /= "ZaqroU";
    return home_dir;
}


bool is_hexstr(const std::string &str)
{
    for (char c : str)
    {
        if (!std::isxdigit(c))
            return false;
    }
    return true;
}
