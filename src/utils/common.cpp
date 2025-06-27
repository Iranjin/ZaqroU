#include "common.h"

#include <cstdlib>
#include <cctype>
#include <codecvt>
#include <locale>
#include <string>
#include <tinyfiledialogs.h>


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


std::string to_utf8(const std::filesystem::path &path)
{
    std::u8string u8str = path.u8string();
    return std::string(u8str.begin(), u8str.end());
}

std::string show_save_file_dialog(const std::string &title,
                                  const std::filesystem::path &default_path,
                                  const std::vector<const char*> &filter_patterns,
                                  const std::string &filter_description)
{
    const char *result = tinyfd_saveFileDialog(title.c_str(),
                                               to_utf8(default_path).c_str(),
                                               static_cast<int>(filter_patterns.size()),
                                               filter_patterns.data(),
                                               filter_description.empty() ? nullptr : filter_description.c_str());
    return result ? std::string(result) : "";
}

std::string show_open_file_dialog(const std::string &title,
                                  const std::filesystem::path &default_path,
                                  const std::vector<const char*> &filter_patterns,
                                  const std::string &filter_description,
                                  bool allow_multiple)
{
    const char *result = tinyfd_openFileDialog(title.c_str(),
                                               to_utf8(default_path).c_str(),
                                               static_cast<int>(filter_patterns.size()),
                                               filter_patterns.data(),
                                               filter_description.empty() ? nullptr : filter_description.c_str(),
                                               allow_multiple ? 1 : 0);
    return result ? std::string(result) : "";
}

