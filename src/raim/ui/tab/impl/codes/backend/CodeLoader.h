#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <lz4.h>

#include <tinyxml2.h>

#include "CodeEntry.h"
#include "CodeEntryManager.h"


class CodeLoader
{
public:
    static void LoadFromFile(const std::string &filename, CodeEntryManager &manager)
    {
        std::ifstream in(filename, std::ios::binary);
        if (!in)
            throw std::runtime_error("Failed to open file.");

        char magic[4];
        in.read(magic, 4);
        if (std::string(magic, 4) != "RAIM")
            throw std::runtime_error("Magic number mismatch.");

        uint8_t version;
        in.read(reinterpret_cast<char *>(&version), 1);

        uint32_t rawSize;
        in.read(reinterpret_cast<char *>(&rawSize), 4);

        std::vector<char> compressedData((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        
        std::vector<char> rawData(rawSize);
        int decompressedSize = LZ4_decompress_safe(compressedData.data(), rawData.data(), compressedData.size(), rawSize);
        if (decompressedSize < 0)
            throw std::runtime_error("Decompression failed.");

        std::istringstream iss(std::string(rawData.data(), rawSize), std::ios::binary);

        uint32_t entryCount;
        iss.read(reinterpret_cast<char *>(&entryCount), 4);

        for (uint32_t i = 0; i < entryCount; ++i)
        {
            CodeEntry entry;

            uint8_t nameLen;
            iss.read(reinterpret_cast<char *>(&nameLen), 1);
            entry.name = readString(iss, nameLen);

            uint16_t codesLen;
            iss.read(reinterpret_cast<char *>(&codesLen), 2);
            entry.codes = readString(iss, codesLen);

            uint8_t authorsLen;
            iss.read(reinterpret_cast<char *>(&authorsLen), 1);
            entry.authors = readString(iss, authorsLen);

            uint8_t flags;
            iss.read(reinterpret_cast<char *>(&flags), 1);
            entry.rawAssembly = flags & 1;
            entry.assemblyRamWrite = flags & 2;
            entry.enabled = flags & 4;

            uint16_t commentLen;
            iss.read(reinterpret_cast<char *>(&commentLen), 2);
            entry.comment = readString(iss, commentLen);

            manager.addCodeEntry(entry);
        }
    }

    static void LoadFromXmlFile(const std::string &filename, CodeEntryManager &manager)
    {
        using namespace tinyxml2;

        XMLDocument doc;
        XMLError result = doc.LoadFile(filename.c_str());
        if (result != XML_SUCCESS)
            throw std::runtime_error("Failed to load XML file.");

        XMLElement *root = doc.FirstChildElement("codes");
        if (root == nullptr)
            throw std::runtime_error("Root element <codes> not found.");

        for (XMLElement *entryElem = root->FirstChildElement("entry");
             entryElem != nullptr;
             entryElem = entryElem->NextSiblingElement("entry"))
        {
            CodeEntry entry;

            const char *nameAttr = entryElem->Attribute("name");
            if (nameAttr == nullptr)
                throw std::runtime_error("<entry> is missing required name attribute.");
            entry.name = nameAttr;

            XMLElement *codeElem = entryElem->FirstChildElement("code");
            if (codeElem != nullptr && codeElem->GetText() != nullptr)
                entry.codes = codeElem->GetText();

            XMLElement *authorsElem = entryElem->FirstChildElement("authors");
            if (authorsElem != nullptr && authorsElem->GetText() != nullptr)
                entry.authors = authorsElem->GetText();

            auto getBool = [](XMLElement *elem, bool &outVal)
            {
                if (elem == nullptr || elem->GetText() == nullptr)
                    return;
                std::string text = elem->GetText();
                outVal = (text == "true" || text == "1");
            };

            XMLElement *rawAsmElem = entryElem->FirstChildElement("raw_assembly");
            getBool(rawAsmElem, entry.rawAssembly);

            XMLElement *ramWriteElem = entryElem->FirstChildElement("assembly_ram_write");
            getBool(ramWriteElem, entry.assemblyRamWrite);

            XMLElement *enabledElem = entryElem->FirstChildElement("enabled");
            getBool(enabledElem, entry.enabled);

            XMLElement *commentElem = entryElem->FirstChildElement("comment");
            if (commentElem != nullptr && commentElem->GetText() != nullptr)
                entry.comment = commentElem->GetText();

            manager.addCodeEntry(entry);
        }
    }

    static void SaveToFile(const std::string &filename, const CodeEntryManager &manager)
    {
        std::ostringstream rawStream(std::ios::binary);

        uint32_t entryCount = manager.size();
        rawStream.write(reinterpret_cast<const char *>(&entryCount), 4);

        const std::vector<CodeEntry> &entries = manager.getEntries();
        for (size_t i = 0; i < entries.size(); ++i)
        {
            const CodeEntry &entry = entries[i];

            uint8_t nameLen = static_cast<uint8_t>(entry.name.size());
            rawStream.write(reinterpret_cast<const char *>(&nameLen), 1);
            rawStream.write(entry.name.c_str(), nameLen);

            uint16_t codeLen = static_cast<uint16_t>(entry.codes.size());
            rawStream.write(reinterpret_cast<const char *>(&codeLen), 2);
            rawStream.write(entry.codes.c_str(), codeLen);

            uint8_t authorsLen = static_cast<uint8_t>(entry.authors.size());
            rawStream.write(reinterpret_cast<const char *>(&authorsLen), 1);
            rawStream.write(entry.authors.c_str(), authorsLen);

            uint8_t flags = 0;
            if (entry.rawAssembly)
                flags |= 1;
            if (entry.assemblyRamWrite)
                flags |= 2;
            if (entry.enabled)
                flags |= 4;
            rawStream.write(reinterpret_cast<const char *>(&flags), 1);

            uint16_t commentLen = static_cast<uint16_t>(entry.comment.size());
            rawStream.write(reinterpret_cast<const char *>(&commentLen), 2);
            rawStream.write(entry.comment.c_str(), commentLen);
        }

        std::string rawDataStr = rawStream.str();
        std::vector<char> rawData(rawDataStr.begin(), rawDataStr.end());

        std::vector<char> compressedData(LZ4_compressBound(rawData.size()));
        int compressedSize = LZ4_compress_default(rawData.data(), compressedData.data(), rawData.size(), compressedData.size());
        if (compressedSize <= 0)
            throw std::runtime_error("Compression failed.");

        std::ofstream out(filename, std::ios::binary);
        if (!out)
            throw std::runtime_error("Failed to open output file.");

        out.write("RAIM", 4);
        uint8_t version = 1;
        out.write(reinterpret_cast<const char *>(&version), 1);
        uint32_t rawSize = static_cast<uint32_t>(rawData.size());
        out.write(reinterpret_cast<const char *>(&rawSize), 4);
        out.write(compressedData.data(), compressedSize);
    }

private:
    static std::string readString(std::istream &in, size_t length)
    {
        std::string str(length, '\0');
        in.read(&str[0], length);
        return str;
    }
};
