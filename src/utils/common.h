#pragma once

#include <imgui.h>

#include <string>


std::string get_home_dir();
std::string get_save_dir();

static bool is_hexstr(const std::string &str);
