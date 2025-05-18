#include "common.h"

#include <cstdlib>


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
