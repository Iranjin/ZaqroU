#include "TitleIdParser.h"

#include <filesystem>
#include <iostream>
#include <cstdio>
#include <vector>

#include "downloader.h"


using namespace tinyxml2;


TitleIdParser::TitleIdParser(const std::string &xml_path)
    : m_path(xml_path)
{
}

bool TitleIdParser::load()
{
    XMLDocument doc;
    if (doc.LoadFile(m_path.c_str()) != XML_SUCCESS)
    {
        std::cerr << "Failed to load XML file: " << m_path << std::endl;
        return false;
    }

    XMLElement *root = doc.FirstChildElement("titles");
    if (!root)
    {
        std::cerr << "No <titles> root element found." << std::endl;
        return false;
    }

    parseTitles(root);
    return true;
}

void TitleIdParser::parseTitles(XMLElement *root)
{
    entries.clear();

    for (XMLElement *title = root->FirstChildElement("title"); title != nullptr; title = title->NextSiblingElement("title"))
    {
        TitleEntry entry;

        const char *idAttr = title->Attribute("id");
        const char *desc = title->FirstChildElement("description") ? title->FirstChildElement("description")->GetText() : nullptr;
        const char *prod = title->FirstChildElement("product") ? title->FirstChildElement("product")->GetText() : nullptr;
        const char *comp = title->FirstChildElement("company") ? title->FirstChildElement("company")->GetText() : nullptr;

        if (idAttr) entry.titleId = idAttr;
        if (desc) entry.description = desc;
        if (prod) entry.product = prod;
        if (comp) entry.company = comp;

        entries.push_back(entry);
    }
}

const std::vector<TitleEntry> &TitleIdParser::getEntries() const
{
    return entries;
}

std::string TitleIdParser::fromUint64(uint64_t titleId)
{
    uint32_t high = (uint32_t) (titleId >> 32);
    uint32_t low = (uint32_t) (titleId & 0xFFFFFFFF);

    char formattedId[20];
    snprintf(formattedId, sizeof(formattedId), "%08X-%08X", high, low);

    return std::string(formattedId);
}

const TitleEntry *TitleIdParser::findEntryByUint64(uint64_t titleId) const
{
    std::string idStr = fromUint64(titleId);

    for (const TitleEntry &entry : entries)
    {
        if (entry.titleId == idStr)
            return &entry;
    }
    return nullptr;
}
