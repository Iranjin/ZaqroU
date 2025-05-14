#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <curl/curl.h>


size_t write_to_buffer(void *ptr, size_t size, size_t nmemb, void *userdata);

bool download_file(const std::string &url, std::vector<char> &data);
bool save_to_file(const std::string &path, const std::vector<char> &data);
