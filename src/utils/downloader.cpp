#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <curl/curl.h>


size_t write_to_buffer(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    std::vector<uint8_t> *buffer = (std::vector<uint8_t> *) userdata;
    size_t totalSize = size * nmemb;
    buffer->insert(buffer->end(), (uint8_t *) ptr, (uint8_t *) ptr + totalSize);
    return totalSize;
}

bool download_file(const std::string &url, std::vector<uint8_t> &data)
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

bool save_to_file(const std::string &path, const std::vector<uint8_t> &data)
{
    std::ofstream file(path, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char *>(data.data()), data.size());
    file.close();

    return true;
}
