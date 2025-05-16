#pragma once

#include <string>


struct CodeEntry
{
    std::string name;
    std::string codes;
    std::string authors;
    bool rawAssembly;
    bool assemblyRamWrite;
    bool enabled;
    std::string comment;

    bool operator==(const CodeEntry &other) const
    {
        return this->name             == other.name && 
               this->codes            == other.codes && 
               this->authors          == other.authors && 
               this->rawAssembly      == other.rawAssembly && 
               this->assemblyRamWrite == other.assemblyRamWrite && 
               this->enabled          == other.enabled && 
               this->comment          == other.comment;
    }
    bool operator!=(const CodeEntry &other) const
    {
        return !(*this == other);
    }
};
