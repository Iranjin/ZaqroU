#pragma once

#include <string>
#include <vector>


std::string wstring_to_string(const std::wstring &wstr);
std::wstring string_to_wstring(const std::string &str);

std::string trim(const std::string &s);
std::vector<std::string> split(const std::string &s, const std::string &delimiter);
std::string join(const std::vector<std::string> &elements, const std::string &delimiter);
