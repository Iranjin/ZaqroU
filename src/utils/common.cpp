#include "common.h"

#include <cstdlib>
#include <cctype>


std::string get_home_dir()
{
#ifdef _WIN32
    return std::getenv("USERPROFILE");
#else
    return std::getenv("HOME");
#endif
}

std::string get_save_dir()
{
    std::string home_dir = get_home_dir();
    home_dir += "/zaqro_u";
    return home_dir;
}


static bool is_hexstr(const std::string &str)
{
    for (char c : str)
    if (!std::isxdigit(c))
        return false;
    return true;
}
