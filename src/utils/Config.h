#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>


class Config
{
private:
    nlohmann::json mData;
    std::string mLoadedFilePath;

public:
    bool load(const std::string &filename);
    bool save(const std::string &filename) const;
    bool save() const;

    bool contains(const std::string &key) const
    {
        return mData.contains(key);
    }
    
    template <typename T>
    void set(const std::string &key, const T &value)
    {
        mData[key] = value;
    }

    template <typename T>
    T get(const std::string &key, const T &default_value = T()) const
    {
        if (mData.contains(key))
        {
            try {
                return mData.at(key).get<T>();
            } catch (...) {
                return default_value;
            }
        }
        return default_value;
    }
    
    bool contains_nested(const std::string &key) const
    {
        std::stringstream ss(key);
        std::string token;
        const nlohmann::json *current = &mData;

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
        nlohmann::json* current = &mData;

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
        const nlohmann::json *current = &mData;

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

    bool empty() const { return mData.empty(); }
    size_t size() const { return mData.size(); }
};
