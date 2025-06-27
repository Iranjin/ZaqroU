#include "CodeLoader.h"

#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

#include <tinyxml2.h>
#include <lz4.h>

#include "CodeEntry.h"
#include "CodeEntryManager.h"


uint8_t CodeLoader::get_version_from_file(const std::filesystem::path &file_path)
{
    std::ifstream in(file_path, std::ios::binary);
    if (!in)
        throw std::runtime_error("Failed to open file.");

    char magic[4];
    in.read(magic, 4);
    if (in.gcount() != 4 || std::string(magic, 4) != "RAIM")
        throw std::runtime_error("Magic number mismatch or file too short.");

    uint8_t version;
    in.read(reinterpret_cast<char*>(&version), 1);
    if (in.gcount() != 1)
        throw std::runtime_error("Failed to read version byte.");

    return version;
}

void CodeLoader::load_from_file(const std::filesystem::path &file_path, CodeEntryManager &manager)
{
    std::ifstream in(file_path, std::ios::binary);
    if (!in)
        throw std::runtime_error("Failed to open file.");

    char magic[4];
    in.read(magic, 4);
    if (std::string(magic, 4) != "RAIM")
        throw std::runtime_error("Magic number mismatch.");

    uint8_t version;
    in.read(reinterpret_cast<char*>(&version), 1);

    if (version > CODELOADER_VERSION)
        throw std::runtime_error("Unsupported version.");

    switch(version)
    {
        case 1:
            load_version_1(in, manager);
            break;
        default:
            throw std::runtime_error("Unsupported version.");
    }
}

void CodeLoader::load_from_xml_file(const std::filesystem::path &file_path, CodeEntryManager &manager)
{
    using namespace tinyxml2;

    XMLDocument doc;
    XMLError result = doc.LoadFile(file_path.string().c_str());
    if (result != XML_SUCCESS)
        throw std::runtime_error("Failed to load XML file.");

    XMLElement *root = doc.FirstChildElement("codes");
    if (root == nullptr)
        throw std::runtime_error("Root element <codes> not found.");

    for (XMLElement *entry_elem = root->FirstChildElement("entry");
            entry_elem != nullptr;
            entry_elem = entry_elem->NextSiblingElement("entry"))
    {
        CodeEntry entry;

        if (const char *name_attr = entry_elem->Attribute("name"))
            entry.name = name_attr;
        else
            throw std::runtime_error("<entry> is missing required name attribute.");

        if (XMLElement *code_elem = entry_elem->FirstChildElement("code");
            code_elem->GetText() != nullptr)
            entry.codes = code_elem->GetText();

        if (XMLElement *authors_elem = entry_elem->FirstChildElement("authors");
            authors_elem->GetText() != nullptr)
            entry.authors = authors_elem->GetText();

        auto get_bool = [](XMLElement *elem, bool &out_val)
        {
            if (elem == nullptr || elem->GetText() == nullptr)
                return;
            std::string text = elem->GetText();
            out_val = (text == "true" || text == "1");
        };

        XMLElement *raw_asm_elem = entry_elem->FirstChildElement("raw_assembly");
        get_bool(raw_asm_elem, entry.raw_assembly);

        XMLElement *raw_write_elem = entry_elem->FirstChildElement("assembly_ram_write");
        get_bool(raw_write_elem, entry.assembly_ram_write);

        XMLElement *enabled_elem = entry_elem->FirstChildElement("enabled");
        get_bool(enabled_elem, entry.enabled);

        if (XMLElement *comment_elem = entry_elem->FirstChildElement("comment");
            comment_elem->GetText() != nullptr)
            entry.comment = comment_elem->GetText();

        manager.add_entry(entry);
    }
}

void CodeLoader::save_to_file(const std::filesystem::path &file_path, const CodeEntryManager &manager)
{
    std::ostringstream raw_stream(std::ios::binary);

    uint32_t entry_count = manager.size();
    raw_stream.write(reinterpret_cast<const char*>(&entry_count), 4);

    const std::vector<CodeEntry> &entries = manager.getEntries();
    for (size_t i = 0; i < entries.size(); ++i)
    {
        const CodeEntry &entry = entries[i];

        uint8_t name_len = static_cast<uint8_t>(entry.name.size());
        raw_stream.write(reinterpret_cast<const char*>(&name_len), 1);
        raw_stream.write(entry.name.c_str(), name_len);

        uint16_t code_len = static_cast<uint16_t>(entry.codes.size());
        raw_stream.write(reinterpret_cast<const char*>(&code_len), 2);
        raw_stream.write(entry.codes.c_str(), code_len);

        uint8_t authors_len = static_cast<uint8_t>(entry.authors.size());
        raw_stream.write(reinterpret_cast<const char*>(&authors_len), 1);
        raw_stream.write(entry.authors.c_str(), authors_len);

        uint8_t flags = 0;
        if (entry.raw_assembly)
            flags |= 1;
        if (entry.assembly_ram_write)
            flags |= 2;
        if (entry.enabled)
            flags |= 4;
        raw_stream.write(reinterpret_cast<const char*>(&flags), 1);

        uint16_t comment_len = static_cast<uint16_t>(entry.comment.size());
        raw_stream.write(reinterpret_cast<const char*>(&comment_len), 2);
        raw_stream.write(entry.comment.c_str(), comment_len);
    }

    std::string raw_data_str = raw_stream.str();
    std::vector<char> raw_data(raw_data_str.begin(), raw_data_str.end());

    std::vector<char> compressed_data(LZ4_compressBound(raw_data.size()));
    int compressed_size = LZ4_compress_default(raw_data.data(), compressed_data.data(), raw_data.size(), compressed_data.size());
    if (compressed_size <= 0)
        throw std::runtime_error("Compression failed.");

    std::ofstream out(file_path, std::ios::binary);
    if (!out)
        throw std::runtime_error("Failed to open output file.");

    out.write("RAIM", 4);
    uint8_t version = CODELOADER_VERSION;
    out.write(reinterpret_cast<const char*>(&version), 1);
    uint32_t rawSize = static_cast<uint32_t>(raw_data.size());
    out.write(reinterpret_cast<const char*>(&rawSize), 4);
    out.write(compressed_data.data(), compressed_size);
}

void CodeLoader::load_version_1(std::istream &in, CodeEntryManager &manager)
{
    uint32_t raw_size;
    in.read(reinterpret_cast<char*>(&raw_size), 4);

    std::vector<char> compressed_data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    std::vector<char> raw_data(raw_size);
    int decompressedSize = LZ4_decompress_safe(compressed_data.data(), raw_data.data(), compressed_data.size(), raw_size);
    if (decompressedSize < 0)
        throw std::runtime_error("Decompression failed.");

    std::istringstream iss(std::string(raw_data.data(), raw_size), std::ios::binary);

    uint32_t entry_count;
    iss.read(reinterpret_cast<char*>(&entry_count), 4);

    for (uint32_t i = 0; i < entry_count; ++i)
    {
        CodeEntry entry;

        uint8_t name_len;
        iss.read(reinterpret_cast<char*>(&name_len), 1);
        entry.name = read_string(iss, name_len);

        uint16_t codes_len;
        iss.read(reinterpret_cast<char*>(&codes_len), 2);
        entry.codes = read_string(iss, codes_len);

        uint8_t authors_len;
        iss.read(reinterpret_cast<char*>(&authors_len), 1);
        entry.authors = read_string(iss, authors_len);

        uint8_t flags;
        iss.read(reinterpret_cast<char*>(&flags), 1);
        entry.raw_assembly = flags & 1;
        entry.assembly_ram_write = flags & 2;
        entry.enabled = flags & 4;

        uint16_t comment_len;
        iss.read(reinterpret_cast<char*>(&comment_len), 2);
        entry.comment = read_string(iss, comment_len);

        manager.add_entry(entry);
    }
}

std::string CodeLoader::read_string(std::istream &in, size_t length)
{
    std::string str(length, '\0');
    in.read(&str[0], length);
    return str;
}
