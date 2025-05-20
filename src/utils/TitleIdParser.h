#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <tinyxml2.h>


struct TitleEntry
{
    std::string title_id;
    std::string description;
    std::string product;
    std::string company;

    std::string getFormattedId() const
    {
        std::string id_part = product.length() >= 4 ? product.substr(product.length() - 4) : product;
        std::string company_part = company.length() >= 2 ? company.substr(company.length() - 2) : company;
        return id_part + company_part;
    }
};

class TitleIdParser
{
private:
    std::string m_path;
    std::vector<TitleEntry> m_entries;

    void parse_titles(tinyxml2::XMLElement *root);

public:
    TitleIdParser(const std::string &xml_path);

    bool load();
    const std::vector<TitleEntry> &get_entries() const;

    static std::string from_Uint64(uint64_t title_id);

    const TitleEntry *find_entry_by_Uint64(uint64_t title_id) const;

    void set_path(const std::string &path) { m_path = path; }
    std::string get_path() const { return m_path; }
};
