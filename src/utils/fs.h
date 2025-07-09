#pragma once

#include <filesystem>
#include <regex>
#include <vector>


std::vector<std::filesystem::path> glob_regex(const std::filesystem::path &root_path, const std::regex &pattern);
