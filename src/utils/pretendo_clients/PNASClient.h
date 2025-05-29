#pragma once

#include "types.h"

#include <tinyxml2.h>
#include <curl/curl.h>

#include <string>
#include <format>


class PNASClient
{
private:
    const char *m_url = "https://account.pretendo.cc";
    const char *m_client_id = "a2efa818a34fa16b8afbc8a74eba3eda";
    const char *m_client_secret = "c91cdb5658bd4954ade78533a339cf9a";

    static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        ((std::string *) userp)->append((char *) contents, size * nmemb);
        return size * nmemb;
    }

    std::string send_request(const std::string &url) const
    {
        CURL *curl = curl_easy_init();
        if (!curl)
            return "";

        std::string readBuffer;

        std::string client_id = std::format("X-Nintendo-Client-ID: {}", m_client_id);
        std::string client_secret = std::format("X-Nintendo-Client-Secret: {}", m_client_secret);

        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, client_id.c_str());
        headers = curl_slist_append(headers, client_secret.c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
            return "";

        return readBuffer;
    }

    static std::string parse_out_id(const std::string &xml)
    {
        tinyxml2::XMLDocument doc;
        if (doc.Parse(xml.c_str()) != tinyxml2::XML_SUCCESS)
            throw std::runtime_error("Failed to parse XML");

        tinyxml2::XMLElement *root = doc.RootElement();
        if (!root)
            throw std::runtime_error("Failed to load XML");

        tinyxml2::XMLElement *mapped_id = root->FirstChildElement("mapped_id");
        if (!mapped_id)
            throw std::runtime_error("No mapped_id element found");

        tinyxml2::XMLElement *out_id = mapped_id->FirstChildElement("out_id");
        if (!out_id)
            throw std::runtime_error("No out_id element found");

        return out_id->GetText();
    }

public:
    PrincipalId get_principal_id(const std::string &account_id) const
    {
        std::string url = std::format("{}/v1/api/admin/mapped_ids?input_type=user_id&output_type=pid&input={}", m_url, account_id);
        std::string xml = send_request(url);
        if (xml.empty())
            return 0;

        std::string pid_str = parse_out_id(xml);
        if (pid_str.empty())
            return 0;

        try {
            return std::stoul(pid_str);
        } catch (...) {
            return 0;
        }
    }

    std::string get_account_id(PrincipalId principal_id) const
    {
        std::string url = std::format("{}/v1/api/admin/mapped_ids?input_type=pid&output_type=user_id&input={}", m_url, principal_id);
        std::string xml = send_request(url);
        if (xml.empty())
            return "";

        return parse_out_id(xml);
    }
};
