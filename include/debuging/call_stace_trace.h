#pragma once

#include <iostream>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <regex>
#include <vector>
// #include <backward.hpp>
#include <tools/format_string.h>
#ifdef LINUX_PLATFORM
#include <execinfo.h>
#include <zconf.h>
#include <link.h>
#endif
#ifdef WINDOWS_PLATFORM
#include <windows.h>
#include <dbghelp.h>
#include <mutex>
#include <dbg
#endif

// #ifdef WINDOWS_PLATFORM
// #define _INITIALIZE_SYM()\
//     ::SymSetOptions( SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES | SYMOPT_UNDNAME );\
//     ::SymInitialize( ::GetCurrentProcess(), "http://msdl.microsoft.com/download/symbols", TRUE );
// #ifndef _INITIALIZE_SYM_CALLED
// #define _INITIALIZE_SYM_CALLED
//     _INITIALIZE_SYM();
// #endif
// #endif

namespace KCore
{
    using namespace std;
    // using namespace backward;

    // std::string getexepath()
    // {
    //     char result[PATH_MAX];
    //     ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    //     return std::string(result, (count > 0) ? count : 0);
    // }

    std::string sh(std::string cmd)
    {
#ifdef LINUX_PLATFORM
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
#elif WINDOWS_PLATFORM

#endif
    }

    size_t convertToVMA(size_t addr)
    {
#ifdef LINUX_PLATFORM
        Dl_info info;
        link_map *link_map;
        dladdr1((void *)addr, &info, (void **)&link_map, RTLD_DL_LINKMAP);
        return addr - link_map->l_addr;
#elif WINDOWS_PLATFORM

#endif
    }

#ifdef WINDOWS_PLATFORM
    once_flag _initialize_sysm_once;
    void _initialize_sym()
    {
        ::SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS | SYMOPT_INCLUDE_32BIT_MODULES | SYMOPT_UNDNAME);
        bool result = ::SymInitialize(::GetCurrentProcess(), "http://msdl.microsoft.com/download/symbols", TRUE);
        // cout << "initialize sysmbol server " << result << endl;
    }
#endif

    // https://stackoverflow.com/a/63856113/4760642
    string get_call_stack(int depth)
    {        
        string call_stack;
#ifdef LINUX_PLATFORM
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
                call_stack.append(result);
                call_stack.append("\n");
                // cout << result << endl;
            }
        }
#elif WINDOWS_PLATFORM
        call_once(_initialize_sysm_once, _initialize_sym);

        PVOID callstack[depth] = {0};
        int frames = (int)CaptureStackBackTrace(1, depth, callstack, NULL);
        // SymInitialize(GetCurrentProcess(), NULL, TRUE);
        cout << "current process : " << GetCurrentProcess() << "id : " << GetCurrentProcessId() << endl;
        for (int i = 0; i < frames; i++)
        {
            // https://github.com/rioki/rex/blob/master/rex/dbg.h#L100
            // cout << (char *)callstack[i] << endl;
            ULONG64 buffer[(sizeof(SYMBOL_INFO) + 1024 + sizeof(ULONG64) - 1) / sizeof(ULONG64)] = {0};
            SYMBOL_INFO *info = (SYMBOL_INFO *)buffer;
            info->SizeOfStruct = sizeof(SYMBOL_INFO);
            info->MaxNameLen = 1024;

            // // Attempt to get information about the symbol and add it to our output parameter.
            DWORD64 displacement = 0;
            // cout<<SymFromAddr(GetCurrentProcess(), (DWORD64)callstack[i], &displacement, info)<<endl;
            if (SymFromAddr(GetCurrentProcess(), (DWORD64)callstack[i], &displacement, info))
            {
                // cout << info->Name << info->NameLen << endl;
                call_stack.append(info->Name);
                call_stack.append(to_string(info->NameLen));
                call_stack.append("\n");
            }
            else
            {
                cout << "cant calculate stack trace" << endl;
            }
        }
#endif
        return call_stack;
    }

}