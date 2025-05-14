#pragma once

#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include "CodeEntry.h"


class CodeEntryManager
{
public:
    void addCodeEntry(const CodeEntry &entry)
    {
        entries.push_back(entry);
    }

    int insertCodeEntry(int index, const CodeEntry &entry)
    {
        if (index >= 0 && index <= entries.size())
        {
            entries.insert(entries.begin() + index, entry);
            return index;
        }
        return -1;
    }

    void removeCodeEntry(const std::string &name)
    {
        entries.erase(std::remove_if(entries.begin(), entries.end(),
                                     [&name](const CodeEntry &entry) { return entry.name == name; }),
                      entries.end());
    }

    void removeCodeEntry(int index)
    {
        if (index >= 0 && index < entries.size())
            entries.erase(entries.begin() + index);
    }

    bool hasEnabledEntry() const
    {
        return std::any_of(entries.begin(), entries.end(),
                           [](const CodeEntry &entry) { return entry.enabled; });
    }

    std::vector<CodeEntry> filterEntries(std::function<bool(const CodeEntry&)> predicate) const
    {
        std::vector<CodeEntry> result;
        std::copy_if(entries.begin(), entries.end(), std::back_inserter(result), predicate);
        return result;
    }

    CodeEntry* findCodeEntry(const std::string &name)
    {
        auto it = std::find_if(entries.begin(), entries.end(),
                               [&name](const CodeEntry &entry) { return entry.name == name; });
        return (it != entries.end()) ? &(*it) : nullptr;
    }

    size_t size() const { return entries.size(); }
    bool empty() const { return entries.empty(); }

    const std::vector<CodeEntry> &getEntries() const { return entries; }

    void clear() { entries.clear(); }

    std::vector<CodeEntry>::iterator begin() { return entries.begin(); }
    std::vector<CodeEntry>::iterator end() { return entries.end(); }
    std::vector<CodeEntry>::const_iterator begin() const { return entries.begin(); }
    std::vector<CodeEntry>::const_iterator end() const { return entries.end(); }

    const CodeEntry &operator[](size_t index) const { return entries[index]; }
    CodeEntry &operator[](size_t index) { return entries[index]; }

private:
    std::vector<CodeEntry> entries;
};
