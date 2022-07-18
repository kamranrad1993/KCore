#pragma once
#include <stdio.h>
#include <iostream>
// #include <execinfo.h>
#include <debuging/call_stace_trace.h>
#include <assert.h>

namespace KCore
{
    using namespace std;

#ifdef LINUX_PLATFORM
#define ANSI_COLOR_RESET "\x1b[0m"

#define ANSI_FOR_COLOR_BLACK "\x1b[30m"
#define ANSI_FOR_COLOR_RED "\x1b[31m"
#define ANSI_FOR_COLOR_GREEN "\x1b[32m"
#define ANSI_FOR_COLOR_YELLOW "\x1b[33m"
#define ANSI_FOR_COLOR_BLUE "\x1b[34m"
#define ANSI_FOR_COLOR_MAGENTA "\x1b[35m"
#define ANSI_FOR_COLOR_CYAN "\x1b[36m"
#define ANSI_FOR_COLOR_WHITE "\x1b[37m"

#define ANSI_BACK_COLOR_BLACK "\x1b[40m"
#define ANSI_BACK_COLOR_RED "\x1b[41m"
#define ANSI_BACK_COLOR_GREEN "\x1b[42m"
#define ANSI_BACK_COLOR_YELLOW "\x1b[43m"
#define ANSI_BACK_COLOR_BLUE "\x1b[44m"
#define ANSI_BACK_COLOR_MAGENTA "\x1b[45m"
#define ANSI_BACK_COLOR_CYAN "\x1b[46m"
#define ANSI_BACK_COLOR_WHITE "\x1b[47m"
#endif
#ifdef WINDOWS_PLATFORM
#define ANSI_COLOR_RESET ""

#define ANSI_FOR_COLOR_BLACK ""
#define ANSI_FOR_COLOR_RED ""
#define ANSI_FOR_COLOR_GREEN ""
#define ANSI_FOR_COLOR_YELLOW ""
#define ANSI_FOR_COLOR_BLUE ""
#define ANSI_FOR_COLOR_MAGENTA ""
#define ANSI_FOR_COLOR_CYAN ""
#define ANSI_FOR_COLOR_WHITE ""

#define ANSI_BACK_COLOR_BLACK ""
#define ANSI_BACK_COLOR_RED ""
#define ANSI_BACK_COLOR_GREEN ""
#define ANSI_BACK_COLOR_YELLOW ""
#define ANSI_BACK_COLOR_BLUE ""
#define ANSI_BACK_COLOR_MAGENTA ""
#define ANSI_BACK_COLOR_CYAN ""
#define ANSI_BACK_COLOR_WHITE ""
#endif

#define NEW_LINE "\n"
#define TAB "\t"
#define SPACE " "
#define COLON ":"

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)
#define LINE_NUM STRINGIZE(__LINE__)

#define _COLOR_RESET() \
    cout << ANSI_COLOR_RESET;

    // #define _LOG(background, foreground, ...)            \
//     cout << background << foreground << __VA_ARGS__; \
//     _COLOR_RESET();

// template<typename Head, typename... Args>
// void _LOG_RECURSE(const Head& head, Args&... args )
// {
//     std::cout << args[0];
//     _LOG_RECURSE(args...);
// }

// #define _LOG(...) _LOG_RECURSE(__VA_ARGS__)

//https://stackoverflow.com/a/29326744/4760642
#define _LOG(background, foreground, ...) std::cout, background, foreground, __VA_ARGS__

    template <typename T>
    std::ostream &operator,(std::ostream &out, const T &t)
    {
        out << t << SPACE;
        return out;
    }

    //overloaded version to handle all those special std::endl and others...
    std::ostream &operator,(std::ostream &out, std::ostream &(*f)(std::ostream &))
    {
        out << f << SPACE;
        return out;
    }

#define _LOG_NEW_LINE(background, foreground, ...) \
    _LOG(background, foreground, __VA_ARGS__);     \
    cout << NEW_LINE;

#define _LOG_TRACE() \
    _LOG(ANSI_BACK_COLOR_BLACK, ANSI_FOR_COLOR_BLUE, __FILE__, COLON, LINE_NUM);

#define _LOG_BACK_TRACE(depth)                            \
    cout << ANSI_BACK_COLOR_BLACK << ANSI_FOR_COLOR_BLUE; \
    cout << get_call_stack(depth);                                \
    _COLOR_RESET();

#define LOG(...) \
    _LOG_NEW_LINE(ANSI_BACK_COLOR_BLACK, ANSI_FOR_COLOR_WHITE, __VA_ARGS__);

#define LOG_WARNING(...) \
    _LOG_NEW_LINE(ANSI_BACK_COLOR_BLACK, ANSI_FOR_COLOR_YELLOW, __VA_ARGS__);

#define LOG_TRACE(...)                                                      \
    _LOG(ANSI_BACK_COLOR_BLACK, ANSI_FOR_COLOR_WHITE, __VA_ARGS__, SPACE);  \
    _LOG_BACK_TRACE(5);                                                     \
    cout << NEW_LINE;

#define LOG_WARNING_TRACE(...)                                              \
    _LOG(ANSI_BACK_COLOR_BLACK, ANSI_FOR_COLOR_YELLOW, __VA_ARGS__, SPACE); \
    _LOG_TRACE();                                                           \
    cout << NEW_LINE;

#define LOG_ERROR(...) \
    _LOG_NEW_LINE(ANSI_BACK_COLOR_BLACK, ANSI_FOR_COLOR_RED, __VA_ARGS__);

#define LOG_ERROR_BACK_TRACE(depth, ...)                                   \
    _LOG_NEW_LINE(ANSI_BACK_COLOR_BLACK, ANSI_FOR_COLOR_RED, __VA_ARGS__); \
    _LOG_BACK_TRACE(depth);

#define LOG_ERROR_EXIT(depth, exit_code, ...) \
    LOG_ERROR_BACK_TRACE(depth, __VA_ARGS__); \
    exit(exit_code);

#define ASSERT(condition, ...) assert(                                         \
    condition ||                                                               \
    (fprintf(stderr, "%s %s", ANSI_BACK_COLOR_BLACK, ANSI_FOR_COLOR_YELLOW) && \
     fprintf(stderr, __VA_ARGS__) &&                                           \
     fprintf(stderr, " at %s:%d\n", __FILE__, __LINE__)) &&                    \
        fprintf(stderr, ANSI_COLOR_RESET));

}