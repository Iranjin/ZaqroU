#include "CodeEntryManager.h"

#include <algorithm>
#include <iterator>


void CodeEntryManager::begin_modify()
{
    if (m_modifying)
        return;
    m_modifying = true;
    m_snapshot_before_modify = m_entries;
}

void CodeEntryManager::end_modify()
{
    if (!m_modifying)
        return;
    m_modifying = false;

    std::vector<Change> changes;
    detect_changes(changes);

    if (!changes.empty())
    {
        m_undo_stack.push_back(changes);
        m_redo_stack.clear();
    }

    m_snapshot_before_modify.clear();
}

void CodeEntryManager::detect_changes(std::vector<Change> &changes)
{
    size_t n = m_entries.size();
    size_t prevN = m_snapshot_before_modify.size();
    size_t maxN = std::max(n, prevN);

    for (size_t i = 0; i < maxN; ++i)
    {
        CodeEntry old_entry = (i < prevN) ? m_snapshot_before_modify[i] : CodeEntry{};
        CodeEntry new_entry = (i < n) ? m_entries[i] : CodeEntry{};

        if (!(old_entry == new_entry))
        {
            changes.push_back({ i, old_entry, new_entry });
        }
    }
}

bool CodeEntryManager::undo()
{
    if (m_undo_stack.empty())
        return false;
    
    std::vector<Change> changes = m_undo_stack.back();
    m_undo_stack.pop_back();

    for (const Change &c : changes)
    {
        if (c.index < m_entries.size())
        {
            if (c.old_value == CodeEntry{})
                m_entries.erase(m_entries.begin() + c.index);
            else
                m_entries[c.index] = c.old_value;
        }
        else if (c.index == m_entries.size() && c.old_value != CodeEntry{})
        {
            m_entries.push_back(c.old_value);
        }
    }

    m_redo_stack.push_back(changes);
    return true;
}

// FIXME: 一回しかredoできない
bool CodeEntryManager::redo()
{
    if (m_redo_stack.empty())
        return false;
    
    std::vector<Change> changes = m_redo_stack.back();
    m_redo_stack.pop_back();

    for (const Change &c : changes)
    {
        if (c.index < m_entries.size())
            m_entries[c.index] = c.new_value;
        else if (c.index == m_entries.size())
            m_entries.push_back(c.new_value);
    }

    m_undo_stack.push_back(changes);
    return true;
}

void CodeEntryManager::add_entry(const CodeEntry &entry)
{
    m_entries.push_back(entry);
}

size_t CodeEntryManager::insert_entry(size_t index, const CodeEntry &entry)
{
    if (index >= 0 && index <= m_entries.size())
    {
        m_entries.insert(m_entries.begin() + index, entry);
        return index;
    }
    return -1;
}

void CodeEntryManager::remove_entry(const std::string &name)
{
    m_entries.erase(std::remove_if(m_entries.begin(), m_entries.end(),
                                   [&name](const CodeEntry &entry) { return entry.name == name; }),
                    m_entries.end());
}

void CodeEntryManager::remove_entry(size_t index)
{
    if (index >= 0 && index < m_entries.size())
        m_entries.erase(m_entries.begin() + index);
}

std::vector<CodeEntry> CodeEntryManager::filter_entries(std::function<bool(const CodeEntry&)> predicate) const
{
    std::vector<CodeEntry> result;
    std::copy_if(m_entries.begin(), m_entries.end(), std::back_inserter(result), predicate);
    return result;
}

CodeEntry *CodeEntryManager::find_entry(const std::string &name)
{
    auto it = std::find_if(m_entries.begin(), m_entries.end(),
                           [&name](const CodeEntry &entry) { return entry.name == name; });
    return (it != m_entries.end()) ? &(*it) : nullptr;
}

bool CodeEntryManager::has_enabled_entry() const
{
    return std::any_of(m_entries.begin(), m_entries.end(),
                       [](const CodeEntry &entry) { return entry.enabled; });
}

size_t CodeEntryManager::sum_size() const
{
    size_t _size = m_entries.size();
    for (const std::vector<Change> &stack : m_undo_stack)
        _size += stack.size();
    for (const std::vector<Change> &stack : m_redo_stack)
        _size += stack.size();
    _size += m_snapshot_before_modify.size();
    return _size;
}
