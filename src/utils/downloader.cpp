#include "downloader.h"

#include <iostream>
#include <fstream>


namespace
{

size_t write_to_buffer(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    std::vector<char> *buffer = (std::vector<char> *) userdata;
    size_t totalSize = size * nmemb;
    buffer->insert(buffer->end(), (char *) ptr, (char *) ptr + totalSize);
    return totalSize;
}

}

bool download_file(const std::string &url, std::vector<char> &data)
{
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_buffer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "Download failed: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        return true;
    }

    std::cerr << "Failed to initialize curl." << std::endl;
    return false;
}

bool save_to_file(const std::filesystem::path &file_path, const std::vector<char> &data)
{
    std::ofstream file(file_path, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return false;
    }

    file.write(data.data(), data.size());
    file.close();

    return true;
}
