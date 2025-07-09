#include "fs.h"

#include <string>


std::vector<std::filesystem::path> glob_regex(const std::filesystem::path &root_path, const std::regex &pattern)
{
    std::vector<std::filesystem::path> result;

    if (!std::filesystem::exists(root_path) || !std::filesystem::is_directory(root_path)) {
        return result;
    }

    for (const std::filesystem::directory_entry &entry : std::filesystem::recursive_directory_iterator(root_path))
    {
        std::string relative = std::filesystem::relative(entry.path(), root_path).string();

        std::replace(relative.begin(), relative.end(), '\\', '/');

        if (std::regex_match(relative, pattern))
            result.push_back(entry.path());
    }

    return result;
}
