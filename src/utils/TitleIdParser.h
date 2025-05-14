#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <tinyxml2.h>


struct TitleEntry
{
    std::string titleId;
    std::string description;
    std::string product;
    std::string company;

    std::string getFormattedId() const
    {
        std::string idPart = product.length() >= 4 ? product.substr(product.length() - 4) : product;
        std::string companyPart = company.length() >= 2 ? company.substr(company.length() - 2) : company;
        return idPart + companyPart;
    }
};

class TitleIdParser
{
private:
    std::string m_path;
    std::vector<TitleEntry> entries;

    void parseTitles(tinyxml2::XMLElement *root);

public:
    TitleIdParser(const std::string &xmlPath);

    bool load();
    const std::vector<TitleEntry> &getEntries() const;

    static std::string fromUint64(uint64_t titleId);

    const TitleEntry *findEntryByUint64(uint64_t titleId) const;
};
