#pragma once

#include <imgui.h>
#include <TextEditor.h>

#include <vector>
#include <string>
#include <filesystem>


std::filesystem::path get_home_dir();
std::filesystem::path get_save_dir();

bool is_hexstr(const std::string &str);

std::string to_utf8(const std::filesystem::path &path);
std::string show_save_file_dialog(const std::string &title,
                                  const std::filesystem::path &default_path,
                                  const std::vector<const char*> &filter_patterns,
                                  const std::string &filter_description = "");
std::string show_open_file_dialog(const std::string &title,
                                  const std::filesystem::path &default_path,
                                  const std::vector<const char*> &filter_patterns,
                                  const std::string &filter_description = "",
                                  bool allow_multiple = false);

void apply_editor_color(TextEditor &editor);
