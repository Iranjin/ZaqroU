#pragma once

#include <string>
#include <vector>


struct CodeEntry
{
    std::string name;
    std::string codes;
    std::string authors;
    bool rawAssembly;
    bool assemblyRamWrite;
    bool enabled;
    std::string comment;
};
