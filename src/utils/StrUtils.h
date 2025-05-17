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
        if (wc <= 0x7F) {
            buf[0] = static_cast<char>(wc);
            result.append(buf, 1);
        } else if (wc <= 0x7FF) {
            buf[0] = 0xC0 | ((wc >> 6) & 0x1F);
            buf[1] = 0x80 | (wc & 0x3F);
            result.append(buf, 2);
        } else if (wc <= 0xFFFF) {
            buf[0] = 0xE0 | ((wc >> 12) & 0x0F);
            buf[1] = 0x80 | ((wc >> 6) & 0x3F);
            buf[2] = 0x80 | (wc & 0x3F);
            result.append(buf, 3);
        } else if (wc <= 0x10FFFF) {
            buf[0] = 0xF0 | ((wc >> 18) & 0x07);
            buf[1] = 0x80 | ((wc >> 12) & 0x3F);
            buf[2] = 0x80 | ((wc >> 6) & 0x3F);
            buf[3] = 0x80 | (wc & 0x3F);
            result.append(buf, 4);
        } else {
            throw std::runtime_error("Invalid Unicode code point");
        }
    }
    return result;
#endif
}
