#include "TitleIdParser.h"

#include <iostream>
#include <cstdio>
#include <vector>

#include "downloader.h"


TitleIdParser::TitleIdParser(const std::filesystem::path &xml_path)
    : m_path(xml_path)
{
}

bool TitleIdParser::load()
{
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(m_path.c_str()) != tinyxml2::XML_SUCCESS)
    {
        std::cerr << "Failed to load XML file: " << m_path << std::endl;
        return false;
    }

    tinyxml2::XMLElement *root = doc.FirstChildElement("titles");
    if (!root)
    {
        std::cerr << "No <titles> root element found." << std::endl;
        return false;
    }

    parse_titles(root);
    return true;
}

void TitleIdParser::parse_titles(tinyxml2::XMLElement *root)
{
    m_entries.clear();

    for (tinyxml2::XMLElement *title = root->FirstChildElement("title"); title != nullptr; title = title->NextSiblingElement("title"))
    {
        TitleEntry entry;

        const char *id_attr = title->Attribute("id");
        const char *desc = title->FirstChildElement("description") ? title->FirstChildElement("description")->GetText() : nullptr;
        const char *prod = title->FirstChildElement("product") ? title->FirstChildElement("product")->GetText() : nullptr;
        const char *comp = title->FirstChildElement("company") ? title->FirstChildElement("company")->GetText() : nullptr;

        if (id_attr) entry.title_id = id_attr;
        if (desc) entry.description = desc;
        if (prod) entry.product = prod;
        if (comp) entry.company = comp;

        m_entries.push_back(entry);
    }
}

const std::vector<TitleEntry> &TitleIdParser::get_entries() const
{
    return m_entries;
}

std::string TitleIdParser::from_Uint64(uint64_t title_id)
{
    uint32_t high = (uint32_t) (title_id >> 32);
    uint32_t low = (uint32_t) (title_id & 0xFFFFFFFF);

    char formatted_id[20];
    snprintf(formatted_id, sizeof(formatted_id), "%08X-%08X", high, low);

    return std::string(formatted_id);
}

const TitleEntry *TitleIdParser::find_entry_by_Uint64(uint64_t title_id) const
{
    std::string id_str = from_Uint64(title_id);

    for (const TitleEntry &entry : m_entries)
    {
        if (entry.title_id == id_str)
            return &entry;
    }
    return nullptr;
}
