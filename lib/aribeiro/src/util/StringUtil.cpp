#include "StringUtil.h"

#include <stdio.h> // printf
#include <stdarg.h> //va_start
#include <stdlib.h>
#include <string.h>
#include <wchar.h> // wprintf

#include <algorithm> // transform

#include <cctype> // tolower

namespace aRibeiro {

    //private copy constructores, to avoid copy...
    StringUtil::StringUtil(const StringUtil& v) {}
    void StringUtil::operator=(const StringUtil&v) {}

    StringUtil::StringUtil() {}

    const char* StringUtil::char_ptr()const {
        if (char_buffer.size() == 0)
            return NULL;
        return &char_buffer[0];
    }

    const wchar_t* StringUtil::wchar_ptr()const {
        if (wchar_buffer.size() == 0)
            return NULL;
        return &wchar_buffer[0];
    }

    const char* StringUtil::printf(const char* format, ...) {

        va_list args;

        va_start(args, format);
        char_buffer.resize(vsnprintf(NULL, 0, format, args) + 1);
        va_end(args);

        va_start(args, format);
        int len = vsnprintf(&char_buffer[0], char_buffer.size(), format, args);
        va_end(args);

        return char_ptr();
    }

    const wchar_t* StringUtil::wprintf(const wchar_t* format, ...) {
        va_list args;

        va_start(args, format);
        wchar_buffer.resize(vswprintf(NULL, 0, format, args) + 1);
        va_end(args);

        va_start(args, format);
        int len = vswprintf(&wchar_buffer[0], wchar_buffer.size(), format, args);
        va_end(args);

        return wchar_ptr();
    }

    std::wstring StringUtil::toWString(const std::string &str) {
        return std::wstring(str.begin(), str.end());
    }

    std::string StringUtil::toString(const std::wstring &wstr) {
        return std::string(wstr.begin(), wstr.end());
    }

    bool StringUtil::startsWith(const std::string str, const std::string prefix) {
        return ((prefix.size() <= str.size()) &&
            std::equal(prefix.begin(), prefix.end(), str.begin()));
    }

    bool StringUtil::endsWith(const std::string str, const std::string sufix) {
        return ((sufix.size() <= str.size()) &&
            std::equal(sufix.begin(), sufix.end(), str.begin() + (str.size() - sufix.size())));
    }

    std::string StringUtil::toLower(const std::string str) {
        std::string aux = str;
        std::transform(aux.begin(), aux.end(), aux.begin(), ::tolower);
        return aux;
    }

    bool StringUtil::contains(const std::string str, const std::string v) {
        return strstr(str.c_str(),v.c_str()) != 0;
    }

}
