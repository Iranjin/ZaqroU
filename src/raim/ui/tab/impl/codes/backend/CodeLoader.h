#pragma once

#include <string>
#include <istream>


#define CODELOADER_VERSION 1


class CodeEntryManager;

class CodeLoader
{
public:
    static void load_from_file(const std::string &filename, CodeEntryManager &manager);
    static void load_from_xml_file(const std::string &filename, CodeEntryManager &manager);
    static void save_to_file(const std::string &filename, const CodeEntryManager &manager);

private:
    static void load_version_1(std::istream &in, CodeEntryManager &manager);
    static std::string readString(std::istream &in, size_t length);
};
