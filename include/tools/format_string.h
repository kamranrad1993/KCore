#pragma once

#include <string>
#include <vector>

namespace KCore
{

    using namespace std;

    // https://stackoverflow.com/a/49812018/4760642
#ifdef LINUX_PLATFORM
    const string vformat(const char *const zcFormat, ...)
    {
        // initialize use of the variable argument array
        va_list vaArgs;
        va_start(vaArgs, zcFormat);

        // reliably acquire the size
        // from a copy of the variable argument array
        // and a functionally reliable call to mock the formatting
        va_list vaArgsCopy;
        va_copy(vaArgsCopy, vaArgs);
        const int iLen = vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
        va_end(vaArgsCopy);

        // return a formatted string without risking memory mismanagement
        // and without assuming any compiler or platform specific behavior
        vector<char> zc(iLen + 1);
        vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
        va_end(vaArgs);
        return string(zc.data(), iLen);
    }
#endif

#ifdef WINDOWS_PLATFORM
        template<typename... Args>
        std::string vformat(const char* zcFormat, Args... args)
        {
            size_t size = snprintf(nullptr, 0, zcFormat, args...);
            std::string buf;
            buf.reserve(size + 1);
            buf.resize(size);
            snprintf(&buf[0], size + 1, zcFormat, args...);
            return buf;
        }
#endif
}