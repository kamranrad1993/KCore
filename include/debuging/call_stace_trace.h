#pragma once

#include <array>
#include <errno.h>
#include <iostream>
#include <regex>
#include <stdlib.h>
#include <string.h>
#include <tools/format_string.h>
#include <unistd.h>
#include <vector>
#ifdef LINUX_PLATFORM
#include <execinfo.h>
#include <link.h>
#include <zconf.h>
#endif
#ifdef WINDOWS_PLATFORM
#include <dbghelp.h>
#include <intrin.h>
#include <mutex>
#include <sstream>
#include <stdio.h>
#endif
#ifdef ANDROID_PLATFORM
#include <dlfcn.h>
#include <unwind.h>
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
#endif
        return "";
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
        return 0;
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
#if WINDOWS_PLATFORM

    inline std::string basename(const std::string &file)
    {
        size_t i = file.find_last_of("\\/");
        if (i == std::string::npos)
        {
            return file;
        }
        else
        {
            return file.substr(i + 1);
        }
    }

    struct StackFrame
    {
        DWORD64 address;
        std::string name;
        std::string module;
        unsigned int line;
        std::string file;
    };

    std::vector<StackFrame> stack_trace()
    {
#if _WIN64
        DWORD machine = IMAGE_FILE_MACHINE_AMD64;
#else
        DWORD machine = IMAGE_FILE_MACHINE_I386;
#endif
        HANDLE process = GetCurrentProcess();
        HANDLE thread = GetCurrentThread();

        if (SymInitialize(process, NULL, TRUE) == FALSE)
        {
            cout << __FUNCTION__ << ": Failed to call SymInitialize." << endl;
            return std::vector<StackFrame>();
        }

        SymSetOptions(SYMOPT_LOAD_LINES);

        CONTEXT context = {};
        context.ContextFlags = CONTEXT_FULL;
        RtlCaptureContext(&context);

#if _WIN64
        STACKFRAME frame = {};
        frame.AddrPC.Offset = context.Rip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context.Rbp;
        frame.AddrFrame.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context.Rsp;
        frame.AddrStack.Mode = AddrModeFlat;
#else
        STACKFRAME frame = {};
        frame.AddrPC.Offset = context.Eip;
        frame.AddrPC.Mode = AddrModeFlat;
        frame.AddrFrame.Offset = context.Ebp;
        frame.AddrFrame.Mode = AddrModeFlat;
        frame.AddrStack.Offset = context.Esp;
        frame.AddrStack.Mode = AddrModeFlat;
#endif

        bool first = true;

        std::vector<StackFrame> frames;
        while (StackWalk(machine, process, thread, &frame, &context, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL))
        {
            StackFrame f = {};
            f.address = frame.AddrPC.Offset;

#if _WIN64
            DWORD64 moduleBase = 0;
#else
            DWORD moduleBase = 0;
#endif

            moduleBase = SymGetModuleBase(process, frame.AddrPC.Offset);

            char moduelBuff[MAX_PATH];
            if (moduleBase && GetModuleFileNameA((HINSTANCE)moduleBase, moduelBuff, MAX_PATH))
            {
                f.module = basename(moduelBuff);
            }
            else
            {
                f.module = "Unknown Module";
            }
#if _WIN64
            DWORD64 offset = 0;
#else
            DWORD offset = 0;
#endif
            char symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + 255];
            PIMAGEHLP_SYMBOL symbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
            symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL) + 255;
            symbol->MaxNameLength = 254;

            if (SymGetSymFromAddr(process, frame.AddrPC.Offset, &offset, symbol))
            {
                f.name = symbol->Name;
            }
            else
            {
                DWORD error = GetLastError();
                cout << __FUNCTION__ << ": Failed to resolve address 0x" << frame.AddrPC.Offset << error << endl;
                f.name = "Unknown Function";
            }

            IMAGEHLP_LINE line;
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE);

            DWORD offset_ln = 0;
            if (SymGetLineFromAddr(process, frame.AddrPC.Offset, &offset_ln, &line))
            {
                f.file = line.FileName;
                f.line = line.LineNumber;
            }
            else
            {
                DWORD error = GetLastError();
                cout << __FUNCTION__ << ": Failed to resolve line for 0x" << frame.AddrPC.Offset << error << endl;
                f.line = 0;
            }

            if (!first)
            {
                frames.push_back(f);
            }
            first = false;
        }

        SymCleanup(process);

        return frames;
    }
#endif

#ifdef ANDROID_PLATFORM
    //  https://androiderrors.com/android-ndk-getting-the-backtrace/
    struct BacktraceState
    {
        void **current;
        void **end;
    };

    static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context *context, void *arg)
    {
        BacktraceState *state = static_cast<BacktraceState *>(arg);
        uintptr_t pc = _Unwind_GetIP(context);
        if (pc)
        {
            if (state->current == state->end)
            {
                return _URC_END_OF_STACK;
            }
            else
            {
                *state->current++ = reinterpret_cast<void *>(pc);
            }
        }
        return _URC_NO_REASON;
    }

    size_t captureBacktrace(void **buffer, size_t max)
    {
        BacktraceState state = {buffer, buffer + max};
        _Unwind_Backtrace(unwindCallback, &state);

        return state.current - buffer;
    }

    void dumpBacktrace(std::ostream &os, void **buffer, size_t count)
    {
        for (size_t idx = 0; idx < count; ++idx)
        {
            const void *addr = buffer[idx];
            const char *symbol = "";

            Dl_info info;
            if (dladdr(addr, &info) && info.dli_sname)
            {
                symbol = info.dli_sname;
            }

            os << "  #  " << idx << ": " << addr << "  " << symbol << "\n";
        }
    }
#endif

    string get_call_stack(int depth)
    {
        string call_stack_string;
#ifdef LINUX_PLATFORM
        // https://stackoverflow.com/a/63856113/4760642
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
                call_stack_string.append(result);
                call_stack_string.append("\n");
                // cout << result << endl;
            }
        }
#elif WINDOWS_PLATFORM
        // call_once(_initialize_sysm_once, _initialize_sym);

        // PVOID callstack[depth] = {0};
        // int frames = (int)CaptureStackBackTrace(1, depth, callstack, NULL);
        // // SymInitialize(GetCurrentProcess(), NULL, TRUE);
        // cout << "current process : " << GetCurrentProcess() << "id : " << GetCurrentProcessId() << endl;
        // for (int i = 0; i < frames; i++)
        // {
        //     // https://github.com/rioki/rex/blob/master/rex/dbg.h#L100
        //     // cout << (char *)callstack[i] << endl;
        //     ULONG64 buffer[(sizeof(SYMBOL_INFO) + 1024 + sizeof(ULONG64) - 1) / sizeof(ULONG64)] = {0};
        //     SYMBOL_INFO *info = (SYMBOL_INFO *)buffer;
        //     info->SizeOfStruct = sizeof(SYMBOL_INFO);
        //     info->MaxNameLen = 1024;

        //     // // Attempt to get information about the symbol and add it to our output parameter.
        //     DWORD64 displacement = 0;
        //     // cout<<SymFromAddr(GetCurrentProcess(), (DWORD64)callstack[i], &displacement, info)<<endl;
        //     if (SymFromAddr(GetCurrentProcess(), (DWORD64)callstack[i], &displacement, info))
        //     {
        //         // cout << info->Name << info->NameLen << endl;
        //         call_stack_string.append(info->Name);
        //         call_stack_string.append(to_string(info->NameLen));
        //         call_stack_string.append("\n");
        //     }
        //     else
        //     {
        //         cout << "cant calculate stack trace" << endl;
        //     }
        // }

        vector<StackFrame> frames = stack_trace();
        for (StackFrame frame : frames)
        {
            cout << frame.name << ":" << frame.module << ":" << frame.file << ":" << frame.line << endl;
        }
#elif ANDROID_PLATFORM
        void *callstack[depth];
        captureBacktrace(&callstack[0], depth);
        for (size_t idx = 0; idx < depth; ++idx)
        {
            void *addr = callstack[idx];
            const char *symbol = "";

            Dl_info info;
            if (dladdr(addr, &info) && info.dli_sname)
            {
                symbol = info.dli_sname;
            }
            call_stack_string.append(to_string(idx));
            call_stack_string.append(" : ");
            call_stack_string.append(to_string(*static_cast<size_t *>(addr)));
            call_stack_string.append("  ");
            call_stack_string.append(symbol);
            call_stack_string.append("\n");
        }

#endif
        return call_stack_string;
    }
}
