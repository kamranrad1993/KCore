#pragma once

#include <signal.h>
#include <debuging/signal_handler.h>

namespace KCore
{

    inline void traceBackSignalHandler(int signum)
    {
        LOG("Caught Signal : " + to_string(signum));
        printCallStack(16);
        exit(signum);
    }

#define REGISTER_SIGINT_TRACE_BACK() signal(SIGINT, traceBackSignalHandler);
#define REGISTER_SIGILL_TRACE_BACK() signal(SIGILL, traceBackSignalHandler);
#define REGISTER_SIGABRT_TRACE_BACK() signal(SIGABRT, traceBackSignalHandler);
#define REGISTER_SIGFPE_TRACE_BACK() signal(SIGFPE, traceBackSignalHandler);
#define REGISTER_SIGSEGV_TRACE_BACK() signal(SIGSEGV, traceBackSignalHandler);
#define REGISTER_SIGTERM_TRACE_BACK() signal(SIGTERM, traceBackSignalHandler);

#define REGISTER_ALL_SIGNAL_TRACE_BACK() \
    REGISTER_SIGINT_TRACE_BACK();        \
    REGISTER_SIGILL_TRACE_BACK();        \
    REGISTER_SIGABRT_TRACE_BACK();       \
    REGISTER_SIGFPE_TRACE_BACK();        \
    REGISTER_SIGSEGV_TRACE_BACK();       \
    REGISTER_SIGTERM_TRACE_BACK();
}