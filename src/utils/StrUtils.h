#include <string>
#include <locale>
#include <codecvt>
#include <stdexcept>


#if defined(_WIN32)
#include <windows.h>
#endif


std::string wstring_to_string(const std::wstring &wstr)
{
#if defined(_WIN32)
    if (wstr.empty())
        return {};

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int) wstr.size(), nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0)
        throw std::runtime_error("WideCharToMultiByte failed");

    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int) wstr.size(), &result[0], size_needed, nullptr, nullptr);
    return result;
#else
    // POSIX: assuming wchar_t is UTF-32 (Linux/macOS)
    std::string result;
    result.reserve(wstr.size() * 4); // worst case for UTF-8

    for (wchar_t wc : wstr)
    {
        char buf[4];
        if (wc <= 0x7F)
        {
            buf[0] = static_cast<char>(wc);
            result.append(buf, 1);
        }
        else if (wc <= 0x7FF)
        {
            buf[0] = 0xC0 | ((wc >> 6) & 0x1F);
            buf[1] = 0x80 | (wc & 0x3F);
            result.append(buf, 2);
        }
        else if (wc <= 0xFFFF)
        {
            buf[0] = 0xE0 | ((wc >> 12) & 0x0F);
            buf[1] = 0x80 | ((wc >> 6) & 0x3F);
            buf[2] = 0x80 | (wc & 0x3F);
            result.append(buf, 3);
        }
        else if (wc <= 0x10FFFF)
        {
            buf[0] = 0xF0 | ((wc >> 18) & 0x07);
            buf[1] = 0x80 | ((wc >> 12) & 0x3F);
            buf[2] = 0x80 | ((wc >> 6) & 0x3F);
            buf[3] = 0x80 | (wc & 0x3F);
            result.append(buf, 4);
        }
        else
        {
            throw std::runtime_error("Invalid Unicode code point");
        }
    }
    return result;
#endif
}

std::wstring string_to_wstring(const std::string &str)
{
#if defined(_WIN32)
    if (str.empty())
        return {};

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);
    if (size_needed <= 0)
        throw std::runtime_error("MultiByteToWideChar failed");

    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), &result[0], size_needed);
    return result;
#else
    // POSIX: assuming wchar_t is UTF-32 (Linux/macOS)
    std::wstring result;
    size_t i = 0;
    const size_t len = str.size();

    while (i < len)
    {
        uint32_t codepoint = 0;
        unsigned char c = static_cast<unsigned char>(str[i]);

        if (c <= 0x7F)
        {
            codepoint = c;
            i += 1;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            if (i + 1 >= len) throw std::runtime_error("Invalid UTF-8 sequence");
            codepoint = ((c & 0x1F) << 6) | (str[i + 1] & 0x3F);
            i += 2;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            if (i + 2 >= len) throw std::runtime_error("Invalid UTF-8 sequence");
            codepoint = ((c & 0x0F) << 12) |
                        ((str[i + 1] & 0x3F) << 6) |
                        (str[i + 2] & 0x3F);
            i += 3;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            if (i + 3 >= len) throw std::runtime_error("Invalid UTF-8 sequence");
            codepoint = ((c & 0x07) << 18) |
                        ((str[i + 1] & 0x3F) << 12) |
                        ((str[i + 2] & 0x3F) << 6) |
                        (str[i + 3] & 0x3F);
            i += 4;
        }
        else
        {
            throw std::runtime_error("Invalid UTF-8 byte");
        }

        if (codepoint > 0x10FFFF)
            throw std::runtime_error("Invalid Unicode code point");

        result.push_back(static_cast<wchar_t>(codepoint));
    }

    return result;
#endif
}
