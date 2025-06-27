#pragma once

#include <string>
#include <cstdint>
#include <istream>
#include <filesystem>


#define CODELOADER_VERSION 1


class CodeEntryManager;

class CodeLoader
{
public:
    static uint8_t get_version_from_file(const std::filesystem::path &file_path);
    static void load_from_file(const std::filesystem::path &file_path, CodeEntryManager &manager);
    static void load_from_xml_file(const std::filesystem::path &file_path, CodeEntryManager &manager);
    static void save_to_file(const std::filesystem::path &file_path, const CodeEntryManager &manager);

private:
    static void load_version_1(std::istream &in, CodeEntryManager &manager);
    static std::string read_string(std::istream &in, size_t length);
};
