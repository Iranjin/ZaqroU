#pragma once

#include <vector>
#include <string>
#include <functional>

#include "CodeEntry.h"


struct CodeEntry;

class CodeEntryManager
{
private:
    std::vector<CodeEntry> m_entries;

    struct Change
    {
        size_t index;
        CodeEntry old_value;
        CodeEntry new_value;
    };

    std::vector<std::vector<Change>> m_undo_stack;
    std::vector<std::vector<Change>> m_redo_stack;

    bool m_modifying = false;
    std::vector<CodeEntry> m_snapshot_before_modify;

public:
    void begin_modify();
    void end_modify();
    void detect_changes(std::vector<Change> &changes);

    bool undo();
    bool redo();

    void add_entry(const CodeEntry &entry);
    size_t insert_entry(size_t index, const CodeEntry &entry);
    void remove_entry(const std::string &name);
    void remove_entry(size_t index);

    std::vector<CodeEntry> filter_entries(std::function<bool(const CodeEntry&)> predicate) const;
    CodeEntry *find_entry(const std::string &name);

    bool has_enabled_entry() const;
    size_t sum_size() const;

    size_t size() const { return m_entries.size(); }
    bool empty() const { return m_entries.empty(); }

    const std::vector<CodeEntry> &getEntries() const { return m_entries; }

    void clear() { m_entries.clear(); }

    std::vector<CodeEntry>::iterator begin() { return m_entries.begin(); }
    std::vector<CodeEntry>::iterator end() { return m_entries.end(); }
    std::vector<CodeEntry>::const_iterator begin() const { return m_entries.begin(); }
    std::vector<CodeEntry>::const_iterator end() const { return m_entries.end(); }

    const CodeEntry &operator[](size_t index) const { return m_entries[index]; }
    CodeEntry &operator[](size_t index) { return m_entries[index]; }
};
