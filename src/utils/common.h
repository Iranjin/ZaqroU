#pragma once

#include <imgui.h>

#include <string>
#include <filesystem>


std::filesystem::path get_home_dir();
std::filesystem::path get_save_dir();

bool is_hexstr(const std::string &str);
