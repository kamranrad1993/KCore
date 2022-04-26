#pragma once

#include <iostream>
#include <execinfo.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <zconf.h>
#include <link.h>
#include <regex>
#include <vector>
// #include <backward.hpp>
#include <tools/format_string.h>

namespace KCore
{
    using namespace std;
    // using namespace backward;

    std::string getexepath()
    {
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        return std::string(result, (count > 0) ? count : 0);
    }

    std::string sh(std::string cmd)
    {
        std::array<char, 128> buffer;
        std::string result;
        std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe)
            throw std::runtime_error("popen() failed!");
        while (!feof(pipe.get()))
        {
            if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            {
                result += buffer.data();
            }
        }
        return result;
    }

    size_t convertToVMA(size_t addr)
    {
        Dl_info info;
        link_map *link_map;
        dladdr1((void *)addr, &info, (void **)&link_map, RTLD_DL_LINKMAP);
        return addr - link_map->l_addr;
    }

    // https://stackoverflow.com/a/63856113/4760642
    void printCallStack(int depth)
    {
        void *callstack[depth];
        int frame_count = backtrace(callstack, sizeof(callstack) / sizeof(callstack[0]));
        for (int i = 1; i < frame_count; i++)
        {
            Dl_info info;
            if (dladdr(callstack[i], &info))
            {
                // use addr2line; dladdr itself is rarely useful (see doc)
                string fileName(info.dli_fname);
                size_t VMA_addr = convertToVMA((size_t)callstack[i]);
                VMA_addr -= 1; // https://stackoverflow.com/questions/11579509/wrong-line-numbers-from-addr2line/63841497#63841497
                string cmd = vformat("addr2line -e %s -Cifp %zx", info.dli_fname, VMA_addr);
                string result = sh(cmd);
                cout << result << endl;
            }
        }
    }

}