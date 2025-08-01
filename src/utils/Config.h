#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>


class Config
{
private:
    nlohmann::json m_data;
    std::filesystem::path m_loaded_file_path;

public:
    bool load(const std::filesystem::path &file_path);
    bool save(const std::filesystem::path &file_path) const;
    bool save() const;

    bool contains(const std::string &key) const
    {
        return m_data.contains(key);
    }
    
    template <typename T>
    void set(const std::string &key, const T &value)
    {
        m_data[key] = value;
    }

    template <typename T>
    T get(const std::string &key, const T &default_value = T()) const
    {
        if (m_data.contains(key))
        {
            try {
                return m_data.at(key).get<T>();
            } catch (...) {
                return default_value;
            }
        }
        return default_value;
    }

    bool remove(const std::string &key)
    {
        return m_data.erase(key) > 0;
    }
    
    bool contains_nested(const std::string &key) const
    {
        std::stringstream ss(key);
        std::string token;
        const nlohmann::json *current = &m_data;

        while (std::getline(ss, token, '.'))
        {
            if (current->contains(token))
                current = &(*current)[token];
            else
                return false;
        }

        return true;
    }

    template <typename T>
    void set_nested(const std::string &key, const T &value)
    {
        std::stringstream ss(key);
        std::string token;
        nlohmann::json *current = &m_data;

        while (std::getline(ss, token, '.'))
        {
            if (!ss.eof())
                current = &(*current)[token];
            else
                (*current)[token] = value;
        }
    }

    template <typename T>
    T get_nested(const std::string &key, const T &default_value = T()) const {
        std::stringstream ss(key);
        std::string token;
        const nlohmann::json *current = &m_data;

        while (std::getline(ss, token, '.'))
        {
            if (current->contains(token))
                current = &(*current)[token];
            else
                return default_value;
        }

        try {
            return current->get<T>();
        } catch (...) {
            return default_value;
        }
    }

    bool remove_nested(const std::string &key)
    {
        std::stringstream ss(key);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(ss, token, '.'))
            tokens.push_back(token);

        if (tokens.empty()) return false;

        nlohmann::json *current = &m_data;
        for (size_t i = 0; i < tokens.size() - 1; ++i)
        {
            if (current->contains(tokens[i]) && (*current)[tokens[i]].is_object())
                current = &(*current)[tokens[i]];
            else
                return false;
        }

        return current->erase(tokens.back()) > 0;
    }

    bool empty() const { return m_data.empty(); }
    size_t size() const { return m_data.size(); }
};
