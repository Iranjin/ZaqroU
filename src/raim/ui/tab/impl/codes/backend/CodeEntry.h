#pragma once

#include <string>


struct CodeEntry
{
    std::string name;
    std::string codes;
    std::string authors;
    bool raw_assembly;
    bool assembly_ram_write;
    bool enabled;
    std::string comment;

    bool empty() const
    {
        return name.empty() &&
               codes.empty() &&
               authors.empty() &&
               !raw_assembly &&
               !assembly_ram_write &&
               !enabled &&
               comment.empty();
    }

    bool operator==(const CodeEntry &other) const
    {
        return this->name == other.name && 
               this->codes == other.codes && 
               this->authors == other.authors && 
               this->raw_assembly == other.raw_assembly && 
               this->assembly_ram_write == other.assembly_ram_write && 
               this->enabled == other.enabled && 
               this->comment == other.comment;
    }
    bool operator!=(const CodeEntry &other) const
    {
        return !(*this == other);
    }
};
